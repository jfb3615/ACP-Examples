#include "QatDataAnalysis/OptParse.h"
#include "QatDataAnalysis/Hist2D.h"
#include "QatPlotWidgets/PlotView.h"
#include "QatPlotting/PlotStream.h"
#include "QatPlotting/PlotProfile.h"
#include "QatPlotting/PlotKey.h"
#include "QatPlotting/PlotFunction1D.h"
#include "QatGenericFunctions/Variable.h"
#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <cstdlib>
#include <iostream>
#include <string>
#include <random>
#include <algorithm>
#include <vector>
#include <set>
#include <tuple>
using namespace std;
//
// This program computes the probability of percolating on the 2x2 lattice
// and then generates configurations. The result of the simulation is compared
// to the analytic formula in a plot.

//
// The index class labels a site
//
struct Index   {
  Index(unsigned int i, unsigned int j) :i(i),j(j){}
  bool operator==(const Index &right) const { return right.i==i && right.j==j;} 
  bool operator!=(const Index &right) const { return right.i!=i || right.j!=j;} 
  bool operator< (const Index &right) const { return i<right.i || (i==right.i && j<right.j);}
  bool neighbors (const Index &right) const { 
    return 
      (i==right.i && abs(int(j)-int(right.j))==1) ||
      (j==right.j && abs(int(i)-int(right.i))==1)  ;
  }
  unsigned int i;
  unsigned int j;
};

//
// The cluster class aglomerates sites and determines whether they percolate
//
class Cluster {
  
public:
  
  bool percolates(unsigned int gridSize) {
    if (!indexSet.empty()) {
      if ((*indexSet.begin()).i==0 && (*indexSet.rbegin()).i==gridSize-1) return true;
    }
    return false;
  }

  bool contains(const Index & index) const {
    return indexSet.find(index)!=indexSet.end();
  }
  
  void add(const Index & index) {
    indexSet.insert(index);
  }
  
  void add(const Cluster & cluster) {
    //copy(cluster.indexSet.begin(), cluster.indexSet.end(),inserter(indexSet, indexSet.begin() ) );
    set<Index>::const_iterator c=cluster.indexSet.begin();
    while (c!=cluster.indexSet.end()) {
      indexSet.insert(*c);
      c++;
    }
  }
  
private:
  set<Index> indexSet;

  friend ostream & operator << (ostream &, const Cluster &);
};

//
// A ClusterSet agglomerates clusters.
//
class ClusterSet {
public:
  
  void add(const Cluster & cluster) {
    _cluster.push_back(cluster);
  }
  
  const Cluster & operator()(unsigned int i) const {
    return _cluster[i];
  }
  
  Cluster & operator()(unsigned int i) {
    return _cluster[i];
  }
  
  unsigned int clusterIndex (Index index) const {
    for (unsigned int i=0;i<_cluster.size();i++) {
      if (_cluster[i].contains(index)) return i;
    }
    return _cluster.size();
  }
  
  unsigned int size() const {
    return _cluster.size();
  }
  
  void erase(unsigned int i) {
    _cluster.erase(_cluster.begin()+i);
  }
  
private:
  
  std::vector<Cluster> _cluster;
  
};

// Print out an index
ostream & operator << (ostream & o, const Index & I) {
  return o << I.i << "," << I.j;
}

// Print out a cluster
ostream & operator << (ostream & o, const Cluster & c) {
  set<Index>::iterator i=c.indexSet.begin();
  while (i!=c.indexSet.end()) {
    o << *i << " ";
    i++;
  }
  o << endl;
  return o;
}

// Print out a ClusterSet
ostream & operator << (ostream & o, const ClusterSet & cs) {
  for (unsigned int i=0;i<cs.size();i++) {
    o << cs(i) << std::endl;
  }
  return o;
}
  



int main (int argc, char * * argv) {

  string usage= string("usage: ") + argv[0] + " [NRUNS=val/def=100] [N=val/def=10] [NDIV=val/dev=100]"; 
  if (argc>3) {
    cout << usage << endl;
  }
  //
  // Parse the command line:
  //
  NumericInput input;
  input.declare("NRUNS", "Number of experiments", 400);
  input.declare("N",     "Number of grid points", 2);
  input.declare("NDIV",  "Number of p-points to probe", 25);
  try {
    input.optParse(argc, argv);
  }
  catch (exception & e) {
    cout << usage << endl;
    exit(0);
  }
  double n=input.getByName("N");
  double NDIV=input.getByName("NDIV");
  unsigned int N= (unsigned int) (n+0.5);
  unsigned int NRUNS= (unsigned int) (input.getByName("NRUNS")+0.5);

  // Qt Boilerplate
  QApplication     app(argc,argv);
  
  QMainWindow window;
  QToolBar *toolBar=window.addToolBar("Tools");
  QAction  *quitAction=toolBar->addAction("Quit");
  
  quitAction->setShortcut(QKeySequence("q"));
  
  QObject::connect(quitAction, SIGNAL(triggered()), &app, SLOT(quit()));

  // Plot Boundaries
  PRectF rect;
  rect.setXmin(0.0);
  rect.setXmax(1.0);
  rect.setYmin(0.0);
  rect.setYmax(1.2);
  
  // Make a PlotView
  PlotView view(rect);
  view.setFixedHeight(view.width());
  window.setCentralWidget(&view);
  view.setGrid(false);
  view.setXZero(false);
  view.setYZero(false);
  view.setBox(false);

  // Create a PlotProfile and set its properties.
  PlotProfile profile;
  {
    PlotProfile::Properties prop;
    prop.symbolSize=10;
    prop.pen.setWidth(3);
    prop.brush.setStyle(Qt::SolidPattern);
    //prop.brush.setColor("darkRed");
    profile.setProperties(prop);
  }
 
  // Loop over probability p.
  for (double p=0;p<1.0;p+=1.0/NDIV) {
    double nPERC=0.0;
    // Loop over runs
    for (unsigned int k=0;k<NRUNS;k++) {
      //
      // For Random number generation
      //
      random_device dev;
      mt19937 engine(dev());
      binomial_distribution<int> flip(1,p);
      
      //
      // The configuration is held in a 2d histogram:
      //
      Hist2D h2("Anonymous", N, 0, n, N, 0, n);
      for (unsigned int i=0;i<N;i++) {
	for (unsigned int j=0;j<N;j++) {
	  int y=flip(engine);
	  h2.accumulate(i+0.5,j+0.5,y);
	}
      }
      //
      // Run the clustering algorithm
      //
      ClusterSet clusterSet; 
      while (1) {
	bool action=false;
	for (unsigned int I=0;I<h2.nBinsX();I++) {
	  for (unsigned int J=0;J<h2.nBinsY();J++) {
	    if (h2.bin(I,J)>0) {
	      Index p0(I,J);
	      unsigned int i0=clusterSet.clusterIndex(p0);
	      for (unsigned int i=(unsigned int)   max(int(I)-1,0);i<(unsigned int) min(int(I)+2,int(N));i++) {
		for (unsigned int j=(unsigned int) max(int(J)-1,0);j<(unsigned int) min(int(J)+2,int(N));j++) {
		  Index p1(i,j);
		  if (p0.neighbors(p1) && h2.bin(i,j)>0) {
		    unsigned int i1=clusterSet.clusterIndex(p1);
		    if (i0==clusterSet.size() && i1==clusterSet.size()) {
		      Cluster cluster;
		      cluster.add(p0);
		      cluster.add(p1);
		      clusterSet.add(cluster);
		      i0=clusterSet.clusterIndex(p0);
		      action=true;
		    }
		    else if (i0!=i1 && i0==clusterSet.size() && i1!=clusterSet.size()){
		      clusterSet(i1).add(p0);
		      action=true;
		    }
		    else if (i0!=i1 && i1==clusterSet.size() && i0!=clusterSet.size()){
		      clusterSet(i0).add(p1);
		      action=true;
		    }
		    else if (i0!=i1) {
		      clusterSet(i0).add(clusterSet(i1));
		      clusterSet.erase(i1);
		      i0=clusterSet.clusterIndex(p0);
		      action=true;
		    }
		  }
		}
	      }
	    }
	  }
	}
	if (!action) break;
      }
      //
      // Didja percolate?
      //
      for (unsigned int i=0;i<clusterSet.size();i++) {
	if (clusterSet(i).percolates(N)) nPERC+=1.0;
      }
      //
    }
    // Plot the probability of percolation for each point p.
    double xp=nPERC/NRUNS,xq=1-xp;
    profile.addPoint(p,xp, sqrt(xp*xq/NRUNS));
  }
  view.add(&profile);

  // Plot labelling
  if (0) {
    PlotStream titleStream(view.titleTextEdit());
    titleStream << PlotStream::Clear()
		<< PlotStream::Center() 
		<< PlotStream::Family("Sans Serif") 
		<< PlotStream::Size(16)
		<< "Percolation probability"
		<< PlotStream::EndP();
  }
  
  PlotStream xLabelStream(view.xLabelTextEdit());
  xLabelStream << PlotStream::Clear()
	       << PlotStream::Center()
	       << PlotStream::Family("Sans Serif")
	       << PlotStream::Size(16)
	       << "p"
	       << PlotStream::EndP();
  
  PlotStream yLabelStream(view.yLabelTextEdit());
  yLabelStream << PlotStream::Clear()
	       << PlotStream::Center()
	       << PlotStream::Family("Sans Serif")
	       << PlotStream::Size(16)
	       << "P(p,2)"
	       << PlotStream::EndP();

  //
  // Create a function to overlay with the data, but only overlay
  // in case N=2 because it only applies to that case. 
  //
  using namespace Genfun;
  Variable X;
  PlotFunction1D P=X*X*(2-X*X);
  {
    PlotFunction1D::Properties prop;
    prop.pen.setWidth(3);
    //    prop.pen.setColor("darkBlue");
    P.setProperties(prop);
  }
  if (N==2) view.add(&P);

  QFont font;
  font.setPointSize(16);
  PlotKey key(0.1, 1.0);
  key.add(&P, "Exact solution");
  key.add(&profile, "Numerical calculation");
  key.setFont(font);
  view.add(&key);
  // Interact with the user:
  view.show();
  window.show();
  app.exec();
  view.clear();

  
  return 1;
}


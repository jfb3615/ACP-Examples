#include "QatPlotWidgets/PlotView.h"
#include "QatPlotting/PlotStream.h"
#include "QatPlotting/PlotHist1D.h"
#include "QatDataAnalysis/OptParse.h"
#include "QatDataAnalysis/Hist1D.h"
#include "QatGenericFunctions/Variable.h"
#include "QatGenericFunctions/Exp.h"
#include "QatGenericFunctions/Power.h"
#include "QatPlotting/PlotFunction1D.h"
#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <cstdlib>
#include <iostream>
#include <string>
#include <random>
// This point generates 1D configuration on a lattice of size L (adjustable).
// It then clusters this data and plots the cluster number, together with
// a function that describes perfectly (for L-> infty) the cluster number
// distribution.
int main (int argc, char * * argv) {

  using namespace std;
  using namespace Genfun;
  
  string usage= string("usage: ") + argv[0] + "[L=val/def=10000] [p=val/dev=0.5]"; 

  // For command line parsing:
  NumericInput input;
  unsigned int L=10000;
  double       p=0.5;

  // Parse the command line:  
  input.declare("L", "Length of the 1-D chain", L);
  input.declare("p", "probability of site occupation", p);
  try {
    input.optParse(argc,argv);
  }
  catch (const exception & e) {
    cerr << "Error parsing command line" << endl;
    cerr << usage << endl;
    cerr << input.usage() << endl;
    exit(1);
  }

  L = (unsigned int ) (0.5 + input.getByName("L") );
  p = input.getByName("p");
  
  if (argc!=1) {
    cout << usage << endl;
  }

  // Qt Boilerplate
  QApplication     app(argc,argv);
  
  QMainWindow window;
  QToolBar *toolBar=window.addToolBar("Tools");
  QAction  *nextAction=toolBar->addAction("Next");
  
  nextAction->setShortcut(QKeySequence("n"));
  
  QObject::connect(nextAction, SIGNAL(triggered()), &app, SLOT(quit()));
  
  //
  // For random number generation
  //
  random_device dev;
  mt19937       engine(dev());
  bernoulli_distribution B(p);
  
  //
  // Generate the configuration and cluster at the generation time.
  //
  bool started=0;
  unsigned int nc=0;
  Hist1D ncHist("Cluster size", 100, 1, 100);
  for (unsigned int i=0;i<L;i++) {
    bool occupied = B(engine);
    if (started) {
      if (occupied) {
	nc++;
      }
      else {
	ncHist.accumulate(nc);
	started=false;
	nc=0;
      }
    }
    else {
      if (occupied) {
	started=true;
	nc++;
      }
    }
  }
  ncHist*=(1.0/L);

  //
  // A somewhat convoluted way of writing p^n*(1-p)^2:
  //
  Variable N;
  Exp exp;
  Power powerLogP(log(p));
  GENFUNCTION F=powerLogP(exp(N))*(1-p)*(1-p);
  PlotFunction1D pF=F;
  {
    PlotFunction1D::Properties prop;
    prop.pen.setWidth(3);
    prop.pen.setColor("darkRed");
    pF.setProperties(prop);
  }
  
  // Make a PlotView
  PlotView view;
  window.setCentralWidget(&view);

  PlotHist1D ncPlot=ncHist;
  view.setRect(ncPlot.rectHint());
  view.add(&ncPlot);

  view.add(&pF);
  
  // Plot labelling
  PlotStream titleStream(view.titleTextEdit());
  titleStream << PlotStream::Clear()
	      << PlotStream::Center() 
	      << PlotStream::Family("Sans Serif") 
	      << PlotStream::Size(16)
	      << "1 dimensional lattice L=";
  titleStream.stream << L ;
  titleStream << " p=";
  titleStream.stream << p;
  titleStream<< PlotStream::EndP();
  
  
  PlotStream xLabelStream(view.xLabelTextEdit());
  xLabelStream << PlotStream::Clear()
	       << PlotStream::Center()
	       << PlotStream::Family("Sans Serif")
	       << PlotStream::Size(16)
	       << "cluster size s"
	       << PlotStream::EndP();
  
  PlotStream yLabelStream(view.yLabelTextEdit());
  yLabelStream << PlotStream::Clear()
	       << PlotStream::Center()
	       << PlotStream::Family("Sans Serif")
	       << PlotStream::Size(16)
	       << "n"
	       << PlotStream::Sub()
	       << "s"
	       << PlotStream::EndP();
  QFont font=view.yAxisFont();
  font.setPointSize(12);
  view.yAxisFont()=font;

  // Interact with the user:
  view.show();
  window.show();
  app.exec();
  return 1;
}


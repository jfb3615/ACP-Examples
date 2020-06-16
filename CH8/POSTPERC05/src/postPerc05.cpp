#include "QatPlotWidgets/PlotView.h"
#include "QatPlotWidgets/MultipleViewWindow.h"
#include "QatPlotting/PlotStream.h"
#include "QatPlotting/PlotProfile.h"
#include "QatPlotting/PlotFunction1D.h"
#include "QatPlotting/PlotKey.h"
#include "QatPlotting/PlotHist1D.h"
#include "QatGenericFunctions/Erf.h"
#include "QatGenericFunctions/Tanh.h"
#include "QatGenericFunctions/Variable.h"
#include "QatDataAnalysis/asymmetry.h"
#include "QatDataAnalysis/Hist1D.h"
#include "QatDataAnalysis/OptParse.h"
#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <cstdlib>
#include <iostream>
#include <string>
#include <fstream>
#include <random>
#include <memory>
#include "Eigen/Dense"

struct DataPoint {
  double p;
  unsigned int success;
  unsigned int total;
};

typedef std::unique_ptr<Hist1D> Hist1DPtr;


double logLikelihood(const std::vector<DataPoint> & sequence, double pc, double sigma) {
  Genfun::Erf sigmoid;
  Genfun::Variable p;
  Genfun::GENFUNCTION F=(1+sigmoid((p-pc)/sigma))/2.0;
  double lL=0;
  for (unsigned int i=0;i<sequence.size();i++) {
    double p=F(sequence[i].p);
    double M=sequence[i].total;
    double N=sequence[i].success;
    double logProb= lgamma(M+1) - lgamma(N+1) -lgamma(M-N+1)+sequence[i].success*log(p) + (sequence[i].total-sequence[i].success)*log(1-p);
    if (!finite(logProb)) {
    }
    else {
      lL+=logProb;
    }
  }
  return -2*lL;
  
}

class PercoMarkovChain {
  
  
public:
  
  PercoMarkovChain(const std::vector<DataPoint> & sequence, double s0, double s1):sequence(sequence),s0(s0),s1(s1)
  {
  }
  
  double move(Eigen::VectorXd & v, double prob=0.0) {
    
    Eigen::VectorXd vProp=v;
    vProp[0]+=s0*gauss(engine);
    vProp[1]+=s1*gauss(engine);
    
    
    
    double newProb=exp(-0.5*logLikelihood(sequence, vProp[0], vProp[1]));
    if (newProb>prob) {
      v=vProp;
      return newProb;
    }
    else {
      if (flat(engine) < newProb/prob) {
	v=vProp;
	return newProb;
      }
      else {
	return prob;
      }
    }
  }
  
private:
  
  const std::vector<DataPoint>               &sequence;
  std::mt19937                                engine;
  std::uniform_real_distribution<double>      flat;
  std::normal_distribution<double>            gauss;
  double s0,s1;
};





int main (int argc, char * * argv) {
  
  std::string usage= std::string("usage: ") + argv[0] + "[NBINS=val/def=200] [NMCMC=val/def100K] [NBURNIN=val/def10K] [p0=val/def=0.59] [s0=val/def=0.04] [f0=val/def=0.0002] [f1=val/def=0.0002] inputFile"; 

  NumericInput input;
  unsigned int NMCMC=100000;
  unsigned int NBURNIN=10000;
  unsigned int NBINS=200;
  double p0=0.59;
  double s0=0.04;
  double f0=0.0002;
  double f1=0.0002;
  input.declare ("p0", "Starting value of pCritical", p0);
  input.declare ("s0", "Starting value of sigma", s0);
  input.declare ("f0", "stepsize for p0", f0);
  input.declare ("f1", "stepsize for p1", f1);
  input.declare ("NMCMC", "Number of states to generate", NMCMC);
  input.declare ("NBURNIN", "Number of states to discard for burnin", NBURNIN);
  input.declare ("NBINS", "Number of bins in mcmc histograms", NBINS);
  try {
    input.optParse(argc, argv);
  }
  catch (const std::exception & e) {
    input.optParse(argc, argv);
    std::cerr << usage << std::endl;
    std::cerr << input.usage() << std::endl;
    exit(0);
  }
  if (argc >1 && std::string(argv[1])=="-?") {
    std::cout << usage << std::endl;
    std::cout << input.usage() << std::endl;
    exit(0);
  }
  NBINS   = (unsigned int) (0.5+input.getByName("NBINS"));
  NMCMC   = (unsigned int) (0.5+input.getByName("NMCMC"));
  NBURNIN = (unsigned int) (0.5+input.getByName("NBURNIN"));
  p0=input.getByName("p0");
  s0=input.getByName("s0");
  f0=input.getByName("f0");
  f1=input.getByName("f1");
  
  if (argc!=2) {
    std::cout << usage << std::endl;
  }

  QApplication     app(argc,argv);
  
  MultipleViewWindow window;
  QToolBar *toolBar=window.addToolBar("Tools");
  QAction  *quitAction=toolBar->addAction("Quit");
  
  quitAction->setShortcut(QKeySequence("q"));
  
  QObject::connect(quitAction, SIGNAL(triggered()), &app, SLOT(quit()));
  
  double pMin=1.0,pMax=0.0;
  std::vector<DataPoint> sequence;

  double resultP;
  double resultS;
  std::ifstream fileStream(argv[1]);
  DataPoint point;
  while (1) {
    fileStream >> point.p >> point.success >> point.total;
    if (!fileStream) break;
    sequence.push_back(point);
    pMin=std::min(point.p,pMin);
    pMax=std::max(point.p,pMax);
  }

  PlotProfile prof;
  PlotProfile::Properties prop;
  prop.symbolSize=5;
  prop.brush.setStyle(Qt::SolidPattern);
 
  Hist1D pcHist("", NBINS, 0.58, .61);
  Hist1D sgHist("", NBINS, 0.0, .10);
  if (0) prop.brush.setColor("darkRed");
  prof.setProperties(prop);
  for (unsigned int j=0;j<sequence.size();j++) {
    double xmean=0,xplus=0,xminus=0;
    asymmetry(sequence[j].total,
	      sequence[j].success,
	      xmean, xplus, xminus);
    prof.addPoint(QPointF(sequence[j].p, xmean), xplus-xmean, xmean-xminus);
  }
  
  
  Eigen::VectorXd v(2);
  v(0)=p0; // starting values
  v(1)=s0;  // starting values
  
  PercoMarkovChain chain(sequence,f0,f1);
  double prob=0;
  
  int accept=0,tot=0;
  double probOld=prob;
  
  Eigen::Vector2d a=Eigen::Vector2d::Zero();
  Eigen::Matrix2d C=Eigen::Matrix2d::Zero();
  int c=0;
  for (unsigned int i=0;i<NMCMC;i++) {
    prob=chain.move(v,prob);
    if (prob!=probOld) accept++;
    tot++;
    probOld=prob;
    if (i>=NBURNIN) {
      pcHist.accumulate(v(0));
      sgHist.accumulate(v(1));
      a+=v;
      C+=(v*v.transpose());
      c++;
    }
  }
  if (c==0) {
    a=v;
  }
  else {
    a/=c;
    C/=c;
    C-=a*a.transpose();
  }
  std::cout << " Mean: "<< std::endl << a << std::endl;
  std::cout << " Cov:  " << std::endl << C << std::endl;
  std::cout << "p= " << a[0] << "+-" << sqrt(C(0,0)) << std::endl;
  std::cout << "s= " << a[1] << "+-" << sqrt(C(1,1)) << std::endl;
  std::cout << double(accept)/double(tot) << std::endl;
  resultP=a[0];
  resultS=a[1];
  
  prop.brush.setColor("darkRed");
  
  PlotView view1, view2, view3, view4;
  for (PlotView * view : {&view1, &view2, &view3, &view4}) {
    view->setBox(false);
    view->setGrid(false);
    view->setXZero(false);
    view->setYZero(false);
    
  }
  view2.yAxisFont().setPointSize(12);
  view3.yAxisFont().setPointSize(12);

  view1.add(&prof);
  
  PRectF rect;
  rect.setXmin(pMin-0.01);
  rect.setXmax(pMax+0.01);
  rect.setYmin(0.0);
  rect.setYmax(1.0);
  
  view1.setRect(rect);
  
  
  window.add(&view1, "Input data");
  window.add(&view2, "MCMC p0");
  window.add(&view3, "MCMC s0");
  window.add(&view4, "Fit result");
  
  struct Label {
    PlotView * view;
    std::string title;
    std::string xLabel;
    std::string yLabel;
  } labels[]={{&view1, "", "p", "P(p,N)"},
	      {&view2, "", "p₅₀", "states/bin"},
	      {&view3, "", "ơ", "states/bin"},
	      {&view4, "", "p", "P(p,N)"}};
	    
  for (const Label & label : labels ) {
    PlotStream titleStream(label.view->titleTextEdit());
    titleStream << PlotStream::Clear()
		<< PlotStream::Center() 
		<< PlotStream::Family("Sans Serif") 
		<< PlotStream::Size(16)
		<< label.title
		<< PlotStream::EndP();
    
    
    PlotStream xLabelStream(label.view->xLabelTextEdit());
    xLabelStream << PlotStream::Clear()
		 << PlotStream::Center()
		 << PlotStream::Family("Sans Serif")
		 << PlotStream::Size(16)
		 << label.xLabel
		 << PlotStream::EndP();
    
    PlotStream yLabelStream(label.view->yLabelTextEdit());
    yLabelStream << PlotStream::Clear()
		 << PlotStream::Center()
		 << PlotStream::Family("Sans Serif")
		 << PlotStream::Size(16)
		 << label.yLabel
		 << PlotStream::EndP();
  }
  window.show();
  PlotHist1D p2=pcHist;
  view2.setRect(p2.rectHint());
  view2.add(&p2);

  PlotHist1D p3=sgHist;
  view3.setRect(p3.rectHint());
  view3.add(&p3);

  Genfun::Erf sigmoid;
  Genfun::Variable pp;
  Genfun::GENFUNCTION F=(1+sigmoid((pp-resultP)/resultS))/2.0;
  PlotFunction1D pf=F;
  {
    PlotFunction1D::Properties prop;
    prop.pen.setWidth(3);
    pf.setProperties(prop);
  }
  view4.setRect(rect);
  view4.add(&pf);
  
  
  view4.add(&prof);

  PlotKey key(0.6,0.6);
  key.add(&prof, "Input Data");
  key.add(&pf,   "Fit");
  view4.add(&key);
  
  app.exec();
  
  return 1;
}


#include "QatDataAnalysis/OptParse.h"
#include "QatDataAnalysis/Hist1D.h"
#include "QatPlotWidgets/PlotView.h"
#include "QatPlotting/PlotHist1D.h"
#include "QatPlotting/PlotStream.h"
#include "QatPlotting/PlotFunction1D.h"
#include "QatGenericFunctions/Variable.h"
#include "QatGenericFunctions/Power.h"
#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <string>
#include <random>

using namespace std;

int main (int argc, char * * argv) {

  // Automatically generated:-------------------------:

  std::string usage= std::string("usage: ") + argv[0] + " [N=val] [NPOINTS=10000] "; 
  if (argc==1) {
    std::cout << usage << std::endl;
  }

  NumericInput     numeric;
  numeric.declare("N",       "Set Exponent y=x^N",           0.0);
  numeric.declare("NPOINTS", "Number of Points to sample", 10000);
  numeric.optParse(argc,argv);
  double N = numeric.getByName("N");
  int NPOINTS = numeric.getByName<int> ("NPOINTS");

  Genfun::Variable    X;
  Genfun::GENFUNCTION f =  (N+1.0)*Genfun::Power(N);
  Genfun::GENFUNCTION S =  Genfun::Power(1/(N+1)) (X);

  QApplication     app(argc,argv);
  
  QMainWindow window;
  QToolBar *toolBar=window.addToolBar("Tools");
  QAction  *quitAction=toolBar->addAction("Quit");
  
  quitAction->setShortcut(QKeySequence("q"));
  
  QObject::connect(quitAction, SIGNAL(triggered()), &app, SLOT(quit()));
  
  //
  // Random number generation:
  //
  std::random_device dev;
  std::mt19937 engine(dev());
  std::uniform_real_distribution<double> u;

  Hist1D histogram(100, 0.0, 1.0);
  
  double sum=0.0;
  for (int i=0;i<NPOINTS;i++) {
    double x = S(u(engine));
    sum += 1.0/(N+1)*std::pow(x,5.0-N);
    histogram.accumulate(x);
  }   

  histogram *= (1.0/histogram.sum());

  PlotHist1D plot(histogram);
  PlotFunction1D pfunction(histogram.binWidth()*f);
  PRectF rect;
  rect.setXmin(0.0);
  rect.setXmax(1.0);
  rect.setYmin(0.0);
  rect.setYmax(1.2*histogram.binWidth()*f(1.0));
  
  {
    PlotHist1D::Properties prop;
    prop.pen.setColor(QColor("red"));
    prop.pen.setWidth(2);
    plot.setProperties(prop);
  }

  {
    PlotFunction1D::Properties prop;
    prop.pen.setColor(QColor("gray"));
    prop.pen.setWidth(5);
    pfunction.setProperties(prop);
  }

  PlotView view(rect);
  view.add(&pfunction);
  view.add(&plot);
  window.setCentralWidget(&view);

  
  PlotStream titleStream(view.titleTextEdit());
  titleStream << PlotStream::Clear()
	      << PlotStream::Center() 
	      << PlotStream::Family("Sans Serif") 
	      << PlotStream::Size(16)
	      << "Sampling monomial " << "(" << N << "+1)*x"
	      << PlotStream::Super()  << N   
	      << PlotStream::Normal()
	      << PlotStream::EndP();
  
  
  PlotStream xLabelStream(view.xLabelTextEdit());
  xLabelStream << PlotStream::Clear()
	       << PlotStream::Center()
	       << PlotStream::Family("Sans Serif")
	       << PlotStream::Size(16)
	       << "x"
	       << PlotStream::EndP();
  
  PlotStream yLabelStream(view.yLabelTextEdit());
  yLabelStream << PlotStream::Clear()
	       << PlotStream::Center()
	       << PlotStream::Family("Sans Serif")
	       << PlotStream::Size(16)
	       << "(" << N << "+1)*x"  
	       << PlotStream::Super()  << N   
	       << PlotStream::Normal()
	       << PlotStream::EndP();
  
  
  view.show();
  window.show();
  app.exec();
  return 1;
}


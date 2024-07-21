#include "QatPlotWidgets/PlotView.h"
#include "QatPlotting/PlotStream.h"
#include "QatPlotting/PlotHist1D.h"
#include "QatDataAnalysis/OptParse.h"
#include "QatDataAnalysis/Hist1D.h"
#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <cstdlib>
#include <iostream>
#include <string>
#include <random>
int main (int argc, char * * argv) {

  // Automatically generated:-------------------------:

  std::string usage= std::string("usage: ") + argv[0] + "[XS=val] [N=val]"; 

  double xs=5;
  unsigned int N=10000;
  NumericInput input;
  input.declare("XS", "Delta M over Gamma", xs);
  input.declare("N",  "Number of events",   N);
  input.optParse(argc, argv);
  xs=input.getByName("XS");
  N = (unsigned int) (0.5 + input.getByName("N"));
  if (argc!=1) {
    std::cout << usage << std::endl;
  }

  QApplication     app(argc,argv);
  
  QMainWindow window;
  QToolBar *toolBar=window.addToolBar("Tools");
  QAction  *nextAction=toolBar->addAction("Next");
  
  nextAction->setShortcut(QKeySequence("n"));
  
  QObject::connect(nextAction, SIGNAL(triggered()), &app, SLOT(quit()));
  
  PRectF rect;
  rect.setXmin(0.0);
  rect.setXmax(1.0);
  rect.setYmin(0.0);
  rect.setYmax(10.0);
  

  PlotView view(rect);
  window.setCentralWidget(&view);

  Hist1D total  ("", 400, 0.0, 10.0);
  Hist1D mixed  ("", 400, 0.0, 10.0);
  Hist1D unmixed("", 400, 0.0, 10.0);

  PlotHist1D pTotal=total, pMixed=mixed, pUnmixed=unmixed;
  PlotHist1D::Properties prop;
  view.add(&pTotal);
  prop.pen.setColor("darkBlue");
  pMixed.setProperties(prop);
  view.add(&pMixed);
  prop.pen.setColor("darkRed");
  pUnmixed.setProperties(prop);
  view.add(&pUnmixed);

  std::random_device dev;
  std::mt19937 engine(dev());
  std::exponential_distribution<double> exp;
  int nMixed=0;
  for (unsigned int i=0;i<N;i++) {
    double t=exp(engine);
    total.accumulate(t);
    std::bernoulli_distribution flip(pow(sin(xs*t),2));
    bool isMixed=flip(engine);
    if (isMixed) {
      mixed.accumulate(t);
      nMixed++;
    }
    else {
      unmixed.accumulate(t);
    }
  }
  std::cout << nMixed/double(N) << std::endl;

  view.setRect(pTotal.rectHint());
  

  PlotStream titleStream(view.titleTextEdit());
  titleStream << PlotStream::Clear()
	      << PlotStream::Center() 
	      << PlotStream::Family("Sans Serif") 
	      << PlotStream::Size(16)
	      << "Time evolution of B system"
	      << PlotStream::EndP();
  
  
  PlotStream xLabelStream(view.xLabelTextEdit());
  xLabelStream << PlotStream::Clear()
	       << PlotStream::Center()
	       << PlotStream::Family("Sans Serif")
	       << PlotStream::Size(16)
	       << "t [Units of τ]"
	       << PlotStream::EndP();
  
  PlotStream yLabelStream(view.yLabelTextEdit());
  yLabelStream << PlotStream::Clear()
	       << PlotStream::Center()
	       << PlotStream::Family("Sans Serif")
	       << PlotStream::Size(16)
	       << "events/bin"
	       << PlotStream::EndP();
  
  
  view.show();
  window.show();
  app.exec();
  return 1;
}


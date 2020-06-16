#include "QatPlotWidgets/PlotView.h"
#include "QatPlotting/PlotStream.h"
#include "QatPlotting/PlotHist1D.h"
#include "QatDataAnalysis/Hist1D.h"
#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <cstdlib>
#include <iostream>
#include <string>
#include <random>

double getPull() {

  int NPOW=2;
  int NPOINTS=1000;
  //
  // Random number generation:
  //
  std::random_device dev;
  std::mt19937 engine(dev());
  std::uniform_real_distribution<double> flat;

  double sum=0.0,sumSq=0.0;
  for (int i=0;i<NPOINTS;i++) {
    double x = flat(engine);
    double y = pow(x,NPOW);
    sum   += y;
    sumSq += y*y;
  }
  double mean            = sum/NPOINTS;
  double squareOfTheMean = mean*mean;
  double meanOfTheSquare = sumSq/NPOINTS;
  double variance        = meanOfTheSquare-squareOfTheMean;
  double error           = sqrt(variance/NPOINTS);
  return ((NPOW+1)*mean - 1.0)/(NPOW+1)/error;

}
int main (int argc, char * * argv) {

  // Automatically generated:-------------------------:

  std::string usage= std::string("usage: ") + argv[0]; 
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
  rect.setYmax(1.0);
  

  PlotView view(rect);
  view.setGrid(false);
  view.setXZero(false);

  window.setCentralWidget(&view);
  Hist1D h("",100, -10, 10);
  PlotHist1D pH=h;
  for (int i=0;i<50000;i++) {
    h.accumulate(getPull());
  }

  view.add(&pH);
  view.setRect(pH.rectHint());

  PlotStream statStream(view.statTextEdit());
  statStream << PlotStream::Clear() 
	     << PlotStream::Left() 
	     << PlotStream::Family("Sans Serif") 
	     << PlotStream::Size(16)
	     << "Mean:" << h.mean()
	     << " RMS: " << sqrt(h.variance())
	     << PlotStream::EndP();
    
  
  PlotStream titleStream(view.titleTextEdit());
  titleStream << PlotStream::Clear()
	      << PlotStream::Center() 
	      << PlotStream::Family("Sans Serif") 
	      << PlotStream::Size(16)
	      << PlotStream::EndP();
  
  
  PlotStream xLabelStream(view.xLabelTextEdit());
  xLabelStream << PlotStream::Clear()
	       << PlotStream::Center()
	       << PlotStream::Family("Sans Serif")
	       << PlotStream::Size(16)
	       << "Pull (=δ/σ)"
	       << PlotStream::EndP();
  
  PlotStream yLabelStream(view.yLabelTextEdit());
  yLabelStream << PlotStream::Clear()
	       << PlotStream::Center()
	       << PlotStream::Family("Sans Serif")
	       << PlotStream::Size(16)
	       << "Evaluations/bin"
	       << PlotStream::EndP();
  
  std::cout << h.mean() << sqrt(h.variance()) << std::endl;
  view.show();
  window.show();
  app.exec();
  return 1;
}


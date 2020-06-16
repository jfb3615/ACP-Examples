#include "QatPlotWidgets/PlotView.h"
#include "QatPlotting/PlotStream.h"
#include "QatPlotting/PlotHist1D.h"
#include "QatPlotWidgets/MultipleViewWindow.h"
#include "QatDataAnalysis/Hist1D.h"
#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <cstdlib>
#include <iostream>
#include <random>
#include <string>
int main (int argc, char * * argv) {

  // Automatically generated:-------------------------:

  std::string usage= std::string("usage: ") + argv[0]; 
  if (argc!=1) {
    std::cout << usage << std::endl;
  }


  QApplication     app(argc,argv);
  
  MultipleViewWindow window;
  QToolBar *toolBar=window.addToolBar("Tools");
  QAction  *nextAction=toolBar->addAction("Next");
  
  nextAction->setShortcut(QKeySequence("n"));
  
  QObject::connect(nextAction, SIGNAL(triggered()), &app, SLOT(quit()));
  

  PlotView *pview[4]={new PlotView(), new PlotView(), new PlotView(), new PlotView()};

  Hist1D h[]={
    Hist1D("One uniform variate", 1000, -10, 10),
    Hist1D("Two uniform variates", 1000, -10, 10),
    Hist1D("Three uniform variates", 1000, -10, 10),
    Hist1D("Four uniform variates", 1000, -10, 10)};
  std::mt19937 engine;

  
  std::uniform_real_distribution<double> u(-1.0, 1.0);
  for ( int i=0;i<1000000;i++) {
    double x=0;
    for (int j=0;j<4;j++) {
      x+=u(engine);
      h[j].accumulate(x);
    }
  }
  PlotHist1D p[]={h[0],h[1],h[2],h[3]};
  

  unsigned int c=0;
  for (PlotView *view : pview) {
    PRectF rect=p[c].rectHint();
    rect.setYmax(12000);
    view->setRect(rect);
    view->setXZero(false);
    view->setYZero(false);
    view->setBox(false);
    view->setGrid(false);
    view->yAxisFont().setPointSize(12);
    view->add(&p[c]);
    window.add(view, p[c].histogram()->name());
    
    PlotStream titleStream(view->titleTextEdit());
    titleStream << PlotStream::Clear()
		<< PlotStream::Center() 
		<< PlotStream::Family("Sans Serif") 
		<< PlotStream::Size(16)
		<< p[c].histogram()->name()
		<< PlotStream::EndP();
    
    
    PlotStream xLabelStream(view->xLabelTextEdit());
    xLabelStream << PlotStream::Clear()
		 << PlotStream::Center()
		 << PlotStream::Family("Sans Serif")
		 << PlotStream::Size(16)
		 << "x"
		 << PlotStream::EndP();
    
    PlotStream yLabelStream(view->yLabelTextEdit());
    yLabelStream << PlotStream::Clear()
		 << PlotStream::Center()
		 << PlotStream::Family("Sans Serif")
		 << PlotStream::Size(16)
		 << "counts"
		 << PlotStream::EndP();
    
    c++;
  }

  window.show();
  app.exec();    
  return 1;
}


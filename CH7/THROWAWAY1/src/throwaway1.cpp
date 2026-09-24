#include "QatDataAnalysis/Hist1D.h"
#include "QatPlotting/PlotHist1D.h"
#include "QatPlotWidgets/PlotView.h"
#include "QatPlotting/PlotStream.h"
#include "QatGenericFunctions/Variable.h"
#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <cstdlib>
#include <iostream>
#include <string>
#include <random>
using EngineType=std::mt19937;
int main (int argc, char * * argv) {


  // Automatically generated:-------------------------:

  std::string usage= std::string("usage: ") + argv[0]; 
  if (argc!=1) {
    std::cout << usage << std::endl;
  }


  QApplication     app(argc,argv);
  
  QMainWindow window;
  QToolBar *toolBar=window.addToolBar("Tools");
  QAction  *quitAction=toolBar->addAction("Quit");
  
  quitAction->setShortcut(QKeySequence("q"));
  
  QObject::connect(quitAction, SIGNAL(triggered()), &app, SLOT(quit()));

  EngineType e(100);
  std::uniform_real_distribution<double> rand_x(-1, 1),rand_y(0,0.75);

  Hist1D hx("X", 100, -2.0, 2.0);


  Genfun::Variable    X;
  Genfun::GENFUNCTION f=3/4.0*(1-X*X);
	    
  for (int i=0;i<1000000;i++) {
    double x = rand_x(e);
    double y = rand_y(e);
    if (y<f(x)) {
      hx.accumulate(x);
    }
  }
  
  PlotHist1D px=hx;
  PlotView view;
  view.add(&px);
  view.setRect(px.rectHint());

  view.setBox(false);
  view.setGrid(false);
  view.setXZero(false);
  view.setYZero(false);
  view.yAxisFont().setPointSize(12);
  window.setCentralWidget(&view);
  
  PlotStream titleStream(view.titleTextEdit());
  titleStream << "Von Neumann rejection"  << PlotStream::EndP();
  
  
  PlotStream xLabelStream(view.xLabelTextEdit());
  xLabelStream << "x" << PlotStream::EndP();
  
  PlotStream yLabelStream(view.yLabelTextEdit());
  yLabelStream << "ρ(x)" << PlotStream::EndP();
  
  
  view.show();
  window.show();
  app.exec();

  
  return 1;
}


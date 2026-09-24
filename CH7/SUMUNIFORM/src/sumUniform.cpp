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
  QAction  *quitAction=toolBar->addAction("Quit");
  
  quitAction->setShortcut(QKeySequence("q"));
  
  QObject::connect(quitAction, &QAction::triggered, &app, &QApplication::quit);
  
  // Declare four plot views:
  std::vector<PlotView> plotView(4);

  // Declare four histograms. 
  Hist1D h[]={
    Hist1D("One uniform variate", 1000, -10, 10),
    Hist1D("Two uniform variates", 1000, -10, 10),
    Hist1D("Three uniform variates", 1000, -10, 10),
    Hist1D("Four uniform variates", 1000, -10, 10)};
  std::mt19937 engine;

  // Fill the n^th histogram with the sum of
  // n uniform variates.
  std::uniform_real_distribution u(-1.0, 1.0);
  for ( int i=0;i<1000000;i++) {
    double x=0;
    for (int j=0;j<4;j++) {
      x+=u(engine);
      h[j].accumulate(x);
    }
  }

  // Create four plots, one per histogram.
  std::vector<PlotHist1D> plot={h[0],h[1],h[2],h[3]};

  // Add the plots to the plot views and label them:
  unsigned int c=0;
  for (PlotView & view : plotView) {
    PRectF rect=plot[c].rectHint();
    rect.setYmax(12000);
    view.setRect(rect);
    view.setXZero(false);
    view.setYZero(false);
    view.setBox(false);
    view.setGrid(false);
    view.yAxisFont().setPointSize(12);
    view.add(&plot[c]);
    window.add(&view, plot[c].histogram()->name());
    
    PlotStream titleStream(view.titleTextEdit());
    titleStream << plot[c].histogram()->name() << PlotStream::EndP();
    
    
    PlotStream xLabelStream(view.xLabelTextEdit());
    xLabelStream << "x" << PlotStream::EndP();
    
    PlotStream yLabelStream(view.yLabelTextEdit());
    yLabelStream << "counts" << PlotStream::EndP();
    
    c++;
  }

  // Interact:
  window.show();
  app.exec();    
  return 1;
}


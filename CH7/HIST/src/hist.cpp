#include "QatPlotWidgets/PlotView.h"
#include "QatPlotting/PlotStream.h"
#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <cstdlib>
#include <iostream>
#include <string>

#include "QatDataAnalysis/Hist1D.h"
#include "QatPlotting/PlotHist1D.h"
#include <random>

using EngineType=std::mt19937;

int main (int argc, char * * argv) {

  // Automatically generated:-------------------------:

  std::string usage= std::string("usage: ") + argv[0] + "[1|2|3]"; 

  unsigned int N=1;
  if (argc==2) N=atoi(argv[1]);
  else if (argc>2) {
    std::cout << usage << std::endl;
    exit(0);
  }

  QApplication     app(argc,argv);
  
  QMainWindow window;
  QToolBar *toolBar=window.addToolBar("Tools");
  QAction  *quitAction=toolBar->addAction("Quit");
  
  quitAction->setShortcut(QKeySequence("q"));
  
  QObject::connect(quitAction, &QAction::triggered, &app, &QApplication::quit);
  
  EngineType e;
  Hist1D histogram ("Random", 1000, -10.0, 10.0);
  
  std::uniform_real_distribution u1;
  std::normal_distribution       u2;
  std::gamma_distribution        u3(3.0);
  for (int i=0;i<100000;i++) {
    double x= N==3 ? u3(e) : N==2 ? u2(e) : u1(e);
    histogram.accumulate(x);
  }

  PlotHist1D pH=histogram;
  PlotView view(pH.rectHint());
  view.setGrid(false);
  view.setBox(false);
  view.setXZero(false);
  view.setYZero(false);
  view.add(&pH);
  view.yAxisFont().setPointSize(12);
  window.setCentralWidget(&view);

  std::string title[]={"Uniform distribution",
		       "Normal distribution",
		       "Gamma distribution"};
  
  PlotStream titleStream(view.titleTextEdit());
  titleStream << PlotStream::Size(16) << title[N-1] << PlotStream::EndP();
  
  PlotStream xLabelStream(view.xLabelTextEdit());
  xLabelStream << PlotStream::Size(16) << "x" << PlotStream::EndP();
  
  PlotStream yLabelStream(view.yLabelTextEdit());
  yLabelStream 	<< PlotStream::Size(16) << "ρ(x)" << PlotStream::EndP();
  
  std::cout << pH.textSummary() << std::endl;
  view.show();
  window.show();
  app.exec();
  return 1;
}


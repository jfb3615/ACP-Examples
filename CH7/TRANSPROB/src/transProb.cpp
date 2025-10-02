#include "QatPlotWidgets/PlotView.h"
#include "QatDataAnalysis/Hist1D.h"
#include "QatPlotWidgets/MultipleViewWindow.h"
#include "QatPlotting/PlotStream.h"
#include "QatPlotting/PlotHist1D.h"
#include "QatPlotting/PlotProfile.h"
#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <cstdlib>
#include <iostream>
#include <string>
#include <random> 

int main (int argc, char * * argv) {

  std::mt19937 engine;
  std::normal_distribution<double> normal;
  
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
  
  QObject::connect(quitAction, SIGNAL(triggered()), &app, SLOT(quit()));
  
  PRectF rect{-4.0,4.0,-4.0,4.0};
  Hist1D h("X",100, -4, 4);
  Hist1D j("U",100, -4, 4);
  PlotProfile prof;
  PlotProfile::Properties prop;
  prop.symbolSize=1;
  for (int i=0;i<100000;i++) {
    double x=normal(engine);
    double u=2*std::abs(x)*std::tanh(x);
    h.accumulate(x);
    j.accumulate(u);
    if (i<400) prof.addPoint(x,u);
  }
  PlotHist1D p=h;
  PlotHist1D q=j;
  
  PlotView viewX(p.rectHint());
  window.add(&viewX,"X");

  PlotView viewUX(rect);
  window.add(&viewUX,"U vs X");

  PlotView viewU(q.rectHint());
  window.add(&viewU,"U");

  viewX.add(&p);
  viewU.add(&q);
  viewUX.add(&prof);

  struct Labeling{
    std::string xLabel;
    std::string yLabel;
    std::string title;
  };
  std::map<PlotView *, Labeling> labeling=
    {{&viewX,{"X", "ρ(x)", "Random variable X"}},
     {&viewUX,{"X", "U", "U vs. X"}},
      {&viewU,{"U", "ρ(U)", "Random variable U"}}
    };
  
    
  for (PlotView *view : {&viewX,&viewUX,&viewU}) {
    view->setFixedHeight(view->width());
    PlotStream titleStream(view->titleTextEdit());
    titleStream << PlotStream::Clear()
		<< PlotStream::Center() 
		<< PlotStream::Family("Arial") 
		<< PlotStream::Size(16)
		<< labeling[view].title
		<< PlotStream::EndP();
    
    
    PlotStream xLabelStream(view->xLabelTextEdit());
    xLabelStream << PlotStream::Clear()
		 << PlotStream::Center()
		 << PlotStream::Family("Arial")
		 << PlotStream::Size(16)
		 << labeling[view].xLabel
		 << PlotStream::EndP();
    
    PlotStream yLabelStream(view->yLabelTextEdit());
    yLabelStream << PlotStream::Clear()
		 << PlotStream::Center()
		 << PlotStream::Family("Arial")
		 << PlotStream::Size(16)
		 << labeling[view].yLabel
		 << PlotStream::EndP();
  }
    
  
  window.show();
  app.exec();
  return 1;
}


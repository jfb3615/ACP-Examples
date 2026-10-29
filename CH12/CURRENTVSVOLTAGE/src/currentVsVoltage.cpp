#include "QatPlotWidgets/PlotView.h"
#include "QatPlotWidgets/setLightPalette.h"
#include "QatPlotting/PlotStream.h"
#include "QatPlotting/PlotFunction1D.h"
#include "QatGenericFunctions/Sin.h"
#include "QatGenericFunctions/Variable.h"
#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <cstdlib>
#include <iostream>
#include <string>
#include "Circuit/Capacitor.h"

int main (int argc, char * * argv) {

  using namespace circuit;
  // Automatically generated:-------------------------:
  
  std::string usage= std::string("usage: ") + argv[0]; 
  if (argc!=1) {
    std::cout << usage << std::endl;
  }
  
  
  QApplication     app(argc,argv);
  setLightPalette();
  
  QMainWindow window;
  QToolBar *toolBar=window.addToolBar("Tools");
  QAction  *quitAction=toolBar->addAction("Quit");
  
  quitAction->setShortcut(QKeySequence("q"));
  QObject::connect(quitAction, &QAction::triggered, &app, &QApplication::quit);
  
  PlotView view({0.0, 0.01, -2.0, 2.0});
  window.setCentralWidget(&view);

  using namespace Genfun;
  Capacitor C{"C1",0.001};
  Variable    T;
  GENFUNCTION V=Sin()(1000*T);
  std::unique_ptr<AbsFunction>  IPtr=C.currentFromVoltage(V);

  PlotFunction1D pV{V};
  PlotFunction1D pI{*IPtr};

  view.add(&pV);
  view.add(&pI);
  
  PlotStream titleStream(view.titleTextEdit());
  titleStream << PlotStream::Clear()
	      << PlotStream::Center() 
	      << PlotStream::Family("Arial") 
	      << PlotStream::Size(16)
	      << "Capacitor"
	      << PlotStream::EndP();
  
  
  PlotStream xLabelStream(view.xLabelTextEdit());
  xLabelStream << PlotStream::Clear()
	       << PlotStream::Center()
	       << PlotStream::Family("Arial")
	       << PlotStream::Size(16)
	       << "time t"
	       << PlotStream::EndP();
  
  PlotStream yLabelStream(view.yLabelTextEdit());
  yLabelStream << PlotStream::Clear()
	       << PlotStream::Center()
	       << PlotStream::Family("Arial")
	       << PlotStream::Size(16)
	       << "I [amperes]"
	       << PlotStream::EndP();
  
  
  
  window.show();
  app.exec();
  return 1;
}


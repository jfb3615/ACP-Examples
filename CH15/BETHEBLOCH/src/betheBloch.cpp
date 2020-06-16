#include "BetheBlochFormula.h"
#include "QatPlotWidgets/PlotView.h"
#include "QatPlotting/PlotStream.h"
#include "QatPlotting/PlotFunction1D.h"
#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <cstdlib>
#include <iostream>
#include <string>


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
  
  PRectF rect;
  rect.setXmin(0.1);
  rect.setXmax(100.0);
  rect.setYmin(1.0);
  rect.setYmax(10.0);
  

  PlotView view(rect);
  view.setLogX(true);
  view.setLogY(true);
  view.setBox(false);
  window.setCentralWidget(&view);

  using namespace Genfun;
  BetheBloch alBB(13,                // Atomic number
		  26.981539,         // Atomic mass
		  166.0E-6,          // Mean exitation energy MeV
		  105.6583745);      // Mass of muon

  
  PlotFunction1D pAlBB=alBB;
  {
    PlotFunction1D::Properties prop;
    prop.pen.setWidth(3);
    pAlBB.setProperties(prop);
  }
  view.add(&pAlBB);
  
  PlotStream titleStream(view.titleTextEdit());
  titleStream << PlotStream::Clear()
	      << PlotStream::Center() 
	      << PlotStream::Family("Sans Serif") 
	      << PlotStream::Size(16)
	      << "Muons incident upon aluminum"
	      << PlotStream::EndP();
  
  
  PlotStream xLabelStream(view.xLabelTextEdit());
  xLabelStream << PlotStream::Clear()
	       << PlotStream::Center()
	       << PlotStream::Family("Sans Serif")
	       << PlotStream::Size(16)
	       << "βγ=p/M"
	       << PlotStream::EndP();
  
  PlotStream yLabelStream(view.yLabelTextEdit());
  yLabelStream << PlotStream::Clear()
	       << PlotStream::Center()
	       << PlotStream::Family("Sans Serif")
	       << PlotStream::Size(16)
	       << "-〈dE/dx〉[MeV/g/cm²]"
	       << PlotStream::EndP();
  
  
  view.show();
  window.show();
  app.exec();
  return 1;
}


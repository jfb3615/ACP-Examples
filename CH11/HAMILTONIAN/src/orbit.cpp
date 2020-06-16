#include "QatPlotWidgets/PlotView.h"
#include "QatPlotWidgets/PlotHist1DDialog.h"
#include "QatPlotting/PlotStream.h"
#include "QatPlotting/PlotOrbit.h"
#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <cstdlib>
#include <iostream>
#include <string>

using namespace Genfun;

void orbit(QApplication &app, 
	   Genfun::GENFUNCTION  x0, 
	   Genfun::GENFUNCTION  x1,
	   std::string title,
	   std::string xLabel,
	   std::string yLabel) {

  QMainWindow window;
  QToolBar *toolBar=window.addToolBar("Tools");
  QAction  *nextAction=toolBar->addAction("Next");
  
  nextAction->setShortcut(QKeySequence("n"));
  
  QObject::connect(nextAction, SIGNAL(triggered()), &app, SLOT(quit()));

  
  {
    
    PlotView view;
    view.setFixedHeight(view.width());
    view.setGrid(false);
    view.setBox(false);
    PlotOrbit orbit(x0,x1,0,10);
    view.add(&orbit);
    view.setRect(orbit.rectHint());
    window.setCentralWidget(&view);
    
    PlotStream titleStream(view.titleTextEdit());
    titleStream << PlotStream::Clear()
		<< PlotStream::Center() 
		<< PlotStream::Family("Sans Serif") 
		<< PlotStream::Size(16)
		<< title
		<< PlotStream::EndP();
    
    
    PlotStream xLabelStream(view.xLabelTextEdit());
    xLabelStream << PlotStream::Clear()
		 << PlotStream::Center()
		 << PlotStream::Family("Sans Serif")
		 << PlotStream::Size(16)
		 << xLabel
		 << PlotStream::EndP();
    
    PlotStream yLabelStream(view.yLabelTextEdit());
    yLabelStream << PlotStream::Clear()
		 << PlotStream::Center()
		 << PlotStream::Family("Sans Serif")
		 << PlotStream::Size(16)
		 << yLabel
		 << PlotStream::EndP();
    
    
    view.show();
    window.show();
    app.exec();
  }
}

#include "QatPlotWidgets/PlotView.h"
#include "QatPlotWidgets/PlotHist1DDialog.h"
#include "QatPlotting/PlotStream.h"
#include "QatPlotting/PlotFunction1D.h"
#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <cstdlib>
#include <iostream>
#include <string>
using namespace Genfun;

void plot(QApplication &app, Genfun::GENFUNCTION x, Genfun::GENFUNCTION p,
	  std::string title, std::string xLabel, std::string yLabel) {
  
   
  QMainWindow window;
  QToolBar *toolBar=window.addToolBar("Tools");
  QAction  *nextAction=toolBar->addAction("Next");
   
  nextAction->setShortcut(QKeySequence("n"));
  
  QObject::connect(nextAction, SIGNAL(triggered()), &app, SLOT(quit()));
  
  PRectF rect;
  rect.setXmin(0.0);
  rect.setXmax(10.0);
  rect.setYmin(-2.0);
  rect.setYmax(2.0);

  {

    
    
    PlotView view(rect);
    
    PlotFunction1D pX=x,pP=p;
    {
      PlotFunction1D::Properties prop;
      prop.pen.setWidth(3);

      prop.pen.setColor("red");
      pX.setProperties(prop);
      
      prop.pen.setColor("blue");
      pP.setProperties(prop);
    }

    view.add(&pX);
    view.add(&pP);
    
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

void plot(QApplication &app, Genfun::GENFUNCTION x,
	  std::string title, std::string xLabel, std::string yLabel) {
  
   
  QMainWindow window;
  QToolBar *toolBar=window.addToolBar("Tools");
  QAction  *nextAction=toolBar->addAction("Next");
   
  nextAction->setShortcut(QKeySequence("n"));
  
  QObject::connect(nextAction, SIGNAL(triggered()), &app, SLOT(quit()));
  
  PRectF rect;
  rect.setXmin(0.0);
  rect.setXmax(10.0);
  rect.setYmin(-2.0);
  rect.setYmax(2.0);

  {

    
    
    PlotView view(rect);
    
    PlotFunction1D pX=x;
    {
      PlotFunction1D::Properties prop;
      prop.pen.setWidth(3);

      prop.pen.setColor("red");
      pX.setProperties(prop);
      
    }

    view.add(&pX);
    
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

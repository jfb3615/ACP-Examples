#include "QatPlotWidgets/PlotView.h"
#include "QatPlotting/PlotStream.h"
#include "QatPlotting/PlotFunction1D.h"
#include "QatGenericFunctions/Sin.h"
#include "QatGenericFunctions/Cos.h"
#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <cstdlib>
#include <iostream>
#include <string>
int main (int argc, char * * argv) {

  // This is example code demonstrating how to visualize functions
  // using the QAT libraries.

  // It displays the sine function and the cosine function.  


  // This part is "boilerplate" from the Qt package.  It sets up a window
  // which is part of the graphical user interface.  It adds a toolbar
  // with a "Quit" button, and the button has a keyboard accerator: the
  // q key can be pressed to quit the application. Interested users
  // can consult the Qt documentation for more information. 
  
  QApplication     app(argc,argv);
  QMainWindow window;
  QToolBar *toolBar=window.addToolBar("Tools");
  QAction  *quitAction=toolBar->addAction("Quit");
  quitAction->setShortcut(QKeySequence("q"));
  QObject::connect(quitAction, SIGNAL(triggered()), &app, SLOT(quit()));

  // The following code sets the scale for a linear plot:
  PRectF rect;
  rect.setXmin(-5.0);
  rect.setXmax( 5.0);
  rect.setYmin(-1.2);
  rect.setYmax( 1.2);
  
  // Now create a "PlotView" and put it in the window.  This is a set of
  // axes onto which one can plot functions and draw other objects. 
  PlotView view(rect);
  window.setCentralWidget(&view);
  
  // Here we make plots out of the Sin and Cos functions:
  PlotFunction1D pSin=Genfun::Sin();
  {
    // Set plot properties here: thick solid line for Sine
    PlotFunction1D::Properties prop;
    prop.pen.setWidth(3);
    pSin.setProperties(prop);
  }
  
  PlotFunction1D pCos=Genfun::Cos();
  {
    // Set plot properties here: thick dash line for Cosine
    PlotFunction1D::Properties prop;
    prop.pen.setWidth(3);
    prop.pen.setStyle(Qt::DashLine);
    pCos.setProperties(prop);
  }

  // And we add them to the plotter:
  view.add(&pSin);
  view.add(&pCos);
  
  // Give the plot a title:
  PlotStream titleStream(view.titleTextEdit());
  titleStream << PlotStream::Clear()
	      << PlotStream::Center() 
	      << PlotStream::Family("Sans Serif") 
	      << PlotStream::Size(16)
	      << "Harmonic functions sine and cosine"
	      << PlotStream::EndP();
  
  // Label the x-axis
  PlotStream xLabelStream(view.xLabelTextEdit());
  xLabelStream << PlotStream::Clear()
	       << PlotStream::Center()
	       << PlotStream::Family("Sans Serif")
	       << PlotStream::Size(16)
	       << "x (radians)"
	       << PlotStream::EndP();

  // Label the y-axis.
  PlotStream yLabelStream(view.yLabelTextEdit());
  yLabelStream << PlotStream::Clear()
	       << PlotStream::Center()
	       << PlotStream::Family("Sans Serif")
	       << PlotStream::Size(16)
	       << "y"
	       << PlotStream::EndP();
  
  // Show the window and start user interaction:
  window.show();
  app.exec();
  return 1;
}


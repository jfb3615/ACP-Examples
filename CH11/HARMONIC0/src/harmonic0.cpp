#include "QatGenericFunctions/Variable.h"
#include "QatGenericFunctions/RKIntegrator.h"
#include "QatGenericFunctions/FixedConstant.h"
#include "QatGenericFunctions/PhaseSpace.h"
#include "QatGenericFunctions/RKIntegrator.h"

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

using namespace Genfun;

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
  rect.setXmin(0.0);
  rect.setXmax(10.0);
  rect.setYmin(-2.0);
  rect.setYmax(2.0);
  
  //
  // Harmonic oscillator:
  //
  double K=1.0; // Newtons/m
  double M=1.0; // Kg
  Variable X(0,2),P(1,2);

  GENFUNCTION DXDT=P/M;
  GENFUNCTION DPDT=-K*X;

  RKIntegrator integrator;
  integrator.addDiffEquation(&DXDT,"X0",0);
  integrator.addDiffEquation(&DPDT,"P0", 1);

  GENFUNCTION   x = *integrator.getFunction(X);
  GENFUNCTION   p = *integrator.getFunction(P);
  

  PlotView view(rect);
  window.setCentralWidget(&view);

  PlotFunction1D pX=x,pP=p;
  {
    PlotFunction1D::Properties prop;
    prop.pen.setWidth(3.0);
    prop.pen.setColor("red");
    pX.setProperties(prop);
    prop.pen.setColor("blue");
    pP.setProperties(prop);
  }

  PlotFunction1D PlusOne=Genfun::FixedConstant(1.0);
  PlotFunction1D MnusOne=Genfun::FixedConstant(-1.0);

  view.add(&pX);
  view.add(&pP);
  view.add(&PlusOne);
  view.add(&MnusOne);


  PlotStream titleStream(view.titleTextEdit());
  titleStream << PlotStream::Clear()
	      << PlotStream::Center() 
	      << PlotStream::Family("Sans Serif") 
	      << PlotStream::Size(16)
	      << "Harmonic Oscillator"
	      << PlotStream::EndP();
  
  
  PlotStream xLabelStream(view.xLabelTextEdit());
  xLabelStream << PlotStream::Clear()
	       << PlotStream::Center()
	       << PlotStream::Family("Sans Serif")
	       << PlotStream::Size(16)
	       << "t"
	       << PlotStream::EndP();
  
  PlotStream yLabelStream(view.yLabelTextEdit());
  yLabelStream << PlotStream::Clear()
	       << PlotStream::Center()
	       << PlotStream::Family("Sans Serif")
	       << PlotStream::Size(16)
	       << "q and p"
	       << PlotStream::EndP();
  
  
  view.show();
  window.show();
  app.exec();
  return 1;
}


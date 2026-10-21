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
  QObject::connect(quitAction, &QAction::triggered, &app, &QApplication::quit);
  
  //
  // Harmonic oscillator:
  //
  double K=1.0; // Newtons/m
  double M=1.0; // Kg
  Variable X(0,2),P(1,2);
  
  // Differential equations:
  GENFUNCTION DXDT=P/M;
  GENFUNCTION DPDT=-K*X;

  // Integrator:
  RKIntegrator integrator;
  integrator.addDiffEquation(&DXDT,"X0",0);
  integrator.addDiffEquation(&DPDT,"P0", 1);

  // Solution:
  GENFUNCTION   x = *integrator.getFunction(X);
  GENFUNCTION   p = *integrator.getFunction(P);
  
  // Open a view:
  PlotView view({0.0,10.0,-2.0,2.0});
  window.setCentralWidget(&view);

  // Set plot properties:
  PlotFunction1D pX=x,pP=p;
  {
    PlotFunction1D::Properties prop;
    prop.pen.setWidth(3.0);
    prop.pen.setColor("red");
    pX.setProperties(prop);
    prop.pen.setColor("blue");
    pP.setProperties(prop);
  }

  // Add two curves corresponding to maximium amplitude:
  PlotFunction1D PlusOne=Genfun::FixedConstant(1.0);
  PlotFunction1D MnusOne=Genfun::FixedConstant(-1.0);

  // Add the plots to the view:
  view.add(&pX);
  view.add(&pP);
  view.add(&PlusOne);
  view.add(&MnusOne);

  // Labelling
  auto sz16=PlotStream::Size(16);
  PlotStream (view.titleTextEdit())  << sz16 << "Harmonic Oscillator" << PlotStream::EndP();
  PlotStream (view.xLabelTextEdit()) << sz16 << "t"                  << PlotStream::EndP();
  PlotStream (view.yLabelTextEdit()) << sz16 << "q and p"            << PlotStream::EndP();

  // Interact:
  view.show();
  window.show();
  app.exec();
  return 1;
}


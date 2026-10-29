// ----------------------------------------------------------//
//                                                           //
// This is an example of how to write your own Qat Generic   //
// Function. It consist of a class WaveFunction implementing //
// the wave function^2 of a particle in a stepwise potential.// 
// The function is governed by a set of steps, each one      //
// specifying the start-of-step x and the "index of          //
// refraction" sqrt(1-V/E), which may be complex in a        //
// classically forbidden region.                             //
//                                                           //
// It corresponds to an exercise (#7, ch3) but illustrates:  //
//           * A solution not requiring global variables.    //
//           * How to extend QatGenericFunctions             //
//           * Polymorphism in General.                      //
//                                                           //
// Here you have the a small test program to plot the        //
// WaveFunction.                                             //
// ----------------------------------------------------------// 

#include "QatDataAnalysis/OptParse.h"
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
#include <vector>
#include <complex>
#include "WaveFunction.h"
//
using Complex=std::complex<double>;


int main (int argc, char * * argv) {
  
  // Usage:
  std::string usage= std::string("usage: ") + argv[0] + " [v=val/def=0.25] [a=val/def=1.0]"; 
  
  NumericInput input;
  input.declare("v", "Potential v in units of E", 0.25);
  input.declare("a", "Barrier dimension a, units of reduced wavelength", 1.0);
  try {
    input.optParse(argc,argv);
  }
  catch (std::exception & ) {
    std::cerr << usage << std::endl;
    exit(0);
  }
  
  // Check for unknown arguments after option parsing complete:
  if (argc!=1) {
    std::cout << usage << std::endl;
    exit(0);
  }

  // Get the values of parameters a and v
  double a=input.getByName("a");
  double v=input.getByName("v");

  // Set up the interactive application, make a quit command and shorcut:
  QApplication     app(argc,argv);
  QMainWindow window;
  QToolBar *toolBar=window.addToolBar("Tools");
  QAction  *quitAction=toolBar->addAction("Quit");
  quitAction->setShortcut(QKeySequence("q"));
  QObject::connect(quitAction, &QAction::triggered, &app, &QApplication::quit);

  // Make the plot view:
  PlotView view({-10.0, 10.0, 0.0, 5.0});
  window.setCentralWidget(&view);

  // Set th the steps:  start point + "index of refraction" n=sqrt( (E-V)/E).
  std::vector<Genfun::WaveFunction::Step> step={{-2*a,sqrt(Complex(1.0-v))},
						{-1*a,sqrt(Complex(1.0-2.0*v))},
						{+1*a,sqrt(Complex(1.0-v))},
						{+2*a,Complex(1.0)}};
  
  // Make the new function & the new plot.  Add it to the viewer:
  PlotFunction1D   p=Genfun::WaveFunction(step);
  view.add(&p);

  // Do the labeling:
  auto sz16=PlotStream::Size(16);
  PlotStream (view.titleTextEdit())  << sz16 << "Barrier Potential (zero, up, more, less, zero)" << PlotStream::EndP();
  PlotStream (view.xLabelTextEdit()) << sz16 << "x [units of reduced wavelength]" << PlotStream::EndP();
  PlotStream (view.yLabelTextEdit()) << sz16 << "|ψ|²"<< PlotStream::EndP();
  
  // Interact
  view.show();
  window.show();
  return app.exec();
}


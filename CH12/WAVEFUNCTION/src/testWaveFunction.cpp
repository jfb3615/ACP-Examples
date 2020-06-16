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
typedef std::complex<double> Complex;
Complex I(0.0,1.0);


int main (int argc, char * * argv) {
  
  // Automatically generated:-------------------------:
  
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

  if (argc!=1) {
    std::cout << usage << std::endl;
    exit(0);
  }


  double a=input.getByName("a");
  double v=input.getByName("v");

  QApplication     app(argc,argv);
  
  QMainWindow window;
  QToolBar *toolBar=window.addToolBar("Tools");
  QAction  *nextAction=toolBar->addAction("Next");
  
  nextAction->setShortcut(QKeySequence("n"));
  
  QObject::connect(nextAction, SIGNAL(triggered()), &app, SLOT(quit()));
  
  PRectF rect;
  rect.setXmin(-10.0);
  rect.setXmax(10.0);
  rect.setYmin(0.0);
  rect.setYmax(5.0);
  

    

  PlotView view(rect);
  window.setCentralWidget(&view);


  std::vector<Genfun::WaveFunction::Step> step={{-2*a,sqrt(Complex(1.0-v))},
						{-1*a,sqrt(Complex(1.0-2.0*v))},
						{+1*a,sqrt(Complex(1.0-v))},
						{+2*a,Complex(1.0)}};
  
  PlotFunction1D   p=Genfun::WaveFunction(step);
  view.add(&p);

  
  PlotStream titleStream(view.titleTextEdit());
  titleStream << PlotStream::Clear()
	      << PlotStream::Center() 
	      << PlotStream::Family("Sans Serif") 
	      << PlotStream::Size(16)
	      << "Barrier Potential (zero, up, more, less, zero)"
	      << PlotStream::EndP();
  
  
  PlotStream xLabelStream(view.xLabelTextEdit());
  xLabelStream << PlotStream::Clear()
	       << PlotStream::Center()
	       << PlotStream::Family("Sans Serif")
	       << PlotStream::Size(16)
	       << "x [units of reduced wavelength]"
	       << PlotStream::EndP();
  
  PlotStream yLabelStream(view.yLabelTextEdit());
  yLabelStream << PlotStream::Clear()
	       << PlotStream::Center()
	       << PlotStream::Size(16)
	       << PlotStream::Family("Sans Serif")
	       << "|"
	       << PlotStream::Family("Symbol")
	       << "y"
	       << PlotStream::Family("Sans Serif")
	       << "|"
	       << PlotStream::Super()
	       << "2"
	       << PlotStream::Normal()
	       << PlotStream::EndP();
  
  
  view.show();
  window.show();
  app.exec();
  return 1;
}


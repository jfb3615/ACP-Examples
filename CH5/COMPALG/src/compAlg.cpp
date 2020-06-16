#include "QatPlotWidgets/PlotView.h"
#include "QatPlotting/PlotStream.h"
#include "QatPlotting/PlotPoint.h"
#include "QatPlotting/PlotFunction1D.h"
#include "QatGenericFunctions/Variable.h"
#include "QatGenericFunctions/SimpleIntegrator.h"
#include "QatGenericFunctions/RombergIntegrator.h"
#include "QatGenericFunctions/QuadratureRule.h"
#include "QatGenericFunctions/GaussIntegrator.h"
#include "Eigen/Dense"
#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <cstdlib>
#include <iostream>
#include <string>
#include <cmath>
#include <stdexcept>

struct Measurement {
  double x;
  double y;
};



int main (int argc, char * * argv) {

  // Automatically generated:-------------------------:

  std::string usage= std::string("usage: ") + argv[0]; 
  if (argc!=1) {
    std::cout << usage << std::endl;
  }

  QApplication     app(argc,argv);
  
  QMainWindow window;
  QToolBar *toolBar=window.addToolBar("Tools");
  QAction  *nextAction=toolBar->addAction("Next");
  nextAction->setShortcut(QKeySequence("n"));
  
  QObject::connect(nextAction, SIGNAL(triggered()), &app, SLOT(quit()));
  
  PRectF rect;
  rect.setXmin(-1.0);
  rect.setXmax(1.0);
  rect.setYmin(0.0);
  rect.setYmax(1.0);
  

  PlotView view(rect);
  view.setBox(true);
  view.setLabelXSizePercentage(95);
  window.setCentralWidget(&view);
  
  PlotStream titleStream(view.titleTextEdit());
  titleStream << PlotStream::Clear()
	      << PlotStream::Center() 
	      << PlotStream::Family("Sans Serif") 
	      << PlotStream::Size(16)
	      << "Comparison of Integration schemes"
	      << PlotStream::EndP();
  
  
  PlotStream xLabelStream(view.xLabelTextEdit());
  xLabelStream << PlotStream::Clear()
	       << PlotStream::Center()
	       << PlotStream::Family("Sans Serif")
	       << PlotStream::Size(16)
 	       << "x" 
	       << PlotStream::EndP();
  
  PlotStream yLabelStream(view.yLabelTextEdit());
  yLabelStream << PlotStream::Clear()
	       << PlotStream::Center()
	       << PlotStream::Size(16)
 	       << PlotStream::Family("Sans Serif") << "f(x)" 
	       << PlotStream::EndP();


  
  // Create graphical points; put them in the graph
 

  Genfun::Variable X;
  Genfun::GENFUNCTION f = 2.0/(3+X)/(3+X);

  Genfun::TrapezoidRule trap;
  Genfun::SimpsonsRule  simp;

  Genfun::RombergIntegrator         romberg(-1.0, 1.0);
  Genfun::SimpleIntegrator          tr7(-1,1,trap,7), tr9(-1,1,trap,9), tr11(-1,1,trap,11);
  Genfun::SimpleIntegrator          si7(-1,1,simp,7), si9(-1,1,simp,9), si11(-1,1,simp,11);
  Genfun::GaussLegendreRule         g7(7),g9(9),g11(11);
  Genfun::GaussIntegrator           gl7(g7), gl9(g9), gl11(g11);
  PlotStream statBoxStream(view.statTextEdit());
  statBoxStream << PlotStream::Clear()
		<< PlotStream::Left()
		<< PlotStream::Size(12)
		<< PlotStream::Family("Courier")
		<< "NPOINTS      7              9            11\n" 
		<< "Trap    : "; statBoxStream <<  fabs(2*tr7(f)-1) << ' ' << fabs(2*tr9(f)-1) << ' ' << fabs(2*tr11(f)-1) << "\n";  
  statBoxStream << "Simpson : "; statBoxStream <<  fabs(2*si7(f)-1) << ' ' << fabs(2*si9(f)-1) << ' ' << fabs(2*si11(f)-1) << "\n";  
  statBoxStream << "Gauss   : "; statBoxStream <<  fabs(2*gl7(f)-1) << ' ' << fabs(2*gl9(f)-1) << ' ' << fabs(2*gl11(f)-1) << "\n";    
  statBoxStream << "\nRomberg : "; statBoxStream <<  fabs(2*romberg(f)-1) << "\n"; 
  statBoxStream <<  PlotStream::EndP();

  std::cout << romberg.numFunctionCalls() << std::endl;

  PlotFunction1D plot(f);



  view.add(&plot);
  view.show();
  
  window.show();
  app.exec();

  return 1;

}


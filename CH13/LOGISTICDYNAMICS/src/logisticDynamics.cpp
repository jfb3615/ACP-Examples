#include "QatPlotWidgets/PlotView.h"
#include "QatPlotting/PlotStream.h"
#include "QatPlotting/PlotFunction1D.h"
#include "QatPlotting/RealArg.h"
#include "QatDataAnalysis/OptParse.h"
#include "QatGenericFunctions/Variable.h"
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

  NumericInput input;
  input.declare ("mu", "mu parameter", 2.0);
  try {
    input.optParse(argc, argv);
  }
  catch (std::exception & e) {
    std::cout << input.usage() << std::endl;
    std::cout << usage << std::endl;
  }

  if (argc!=1) {
    std::cout << usage << std::endl;
    exit(0);
  }
  double mu=input.getByName("mu");
  using namespace Genfun;
  Variable X;
  GENFUNCTION F=mu*X*(1-X);
  PlotFunction1D PF(F, RealArg::Gt(0) && RealArg::Lt(1));
  PlotFunction1D PX(X, RealArg::Gt(0) && RealArg::Lt(1));
  
  {
    PlotFunction1D::Properties prop;
    prop.pen.setWidth(3);
    prop.pen.setColor("darkRed");
    PF.setProperties(prop);
  }

  QApplication     app(argc,argv);
  
  QMainWindow window;
  QToolBar *toolBar=window.addToolBar("Tools");
  QAction  *nextAction=toolBar->addAction("Next");
  
  nextAction->setShortcut(QKeySequence("n"));
  
  QObject::connect(nextAction, SIGNAL(triggered()), &app, SLOT(quit()));
  
  PRectF rect;
  rect.setXmin(0.0);
  rect.setXmax(1.0);
  rect.setYmin(0.0);
  rect.setYmax(1.0);
  

  PlotView view(rect);
  window.setCentralWidget(&view);

  view.add(&PF);
  view.add(&PX);
  
  std::ostringstream stream;
  stream << "F(x), mu=" << mu;
  
  PlotStream titleStream(view.titleTextEdit());
  titleStream << PlotStream::Clear()
	      << PlotStream::Center() 
	      << PlotStream::Family("Sans Serif") 
	      << PlotStream::Size(16)
	      << stream.str()
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
	       << PlotStream::Family("Sans Serif")
	       << PlotStream::Size(16)
	       << "f(x)"
	       << PlotStream::EndP();
  
  
  view.show();
  window.show();
  app.exec();
  return 1;
}


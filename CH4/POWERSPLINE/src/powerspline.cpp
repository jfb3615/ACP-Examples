#include "QatPlotWidgets/PlotView.h"
#include "QatPlotting/PlotStream.h"
#include "QatPlotting/PlotProfile.h"
#include "QatPlotting/RealArg.h"
#include "QatPlotting/PlotFunction1D.h"
#include "QatGenericFunctions/Variable.h"
#include "QatGenericFunctions/Exp.h"
#include "QatGenericFunctions/CubicSplinePolynomial.h"
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
  rect.setXmin(0.0);
  rect.setXmax(200);
  rect.setYmin(0.001);
  rect.setYmax(1);

  using namespace Genfun;
  Variable X;
  GENFUNCTION Y=1/X;
  PlotFunction1D    pY(Y, RealArg::Gt(0.1));

  CubicSplinePolynomial cFunction, lcFunction;
  
  PlotProfile prof;
  {
    PlotProfile::Properties prop;
    prop.symbolSize=10;
    prop.brush.setStyle(Qt::SolidPattern);
    prop.brush.setColor("darkRed");
    prof.setProperties(prop);
  }
  //
  // Here we generate the points
  //
  for (int j=0;j<10;j++) {
    double x=10.0*j+2;
    double y=1/x;
    prof.addPoint(x,y);
    cFunction.addPoint(x,y);
    lcFunction.addPoint(x,log(y));
  }
  PlotFunction1D pcFunction=cFunction;
  PlotFunction1D plFunction = Exp()(lcFunction);
  {
    PlotFunction1D::Properties prop;
    prop.pen.setWidth(3);
    prop.pen.setColor("darkRed");
    pcFunction.setProperties(prop);
  }
  {
    PlotFunction1D::Properties prop;
    prop.pen.setWidth(3);
    prop.pen.setColor("darkBlue");
    plFunction.setProperties(prop);
  }

  PlotView view(rect);
  view.setLogY(true);
  window.setCentralWidget(&view);

  view.add(&prof);
  view.add(&pY);
  view.add(&pcFunction);
  view.add(&plFunction);
  
  PlotStream titleStream(view.titleTextEdit());
  titleStream << PlotStream::Clear()
	      << PlotStream::Center() 
	      << PlotStream::Family("Sans Serif") 
	      << PlotStream::Size(16)
	      << "Test interpolations"
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
	       << "y"
	       << PlotStream::EndP();
  PlotStream statLabelStream(view.statTextEdit());
  statLabelStream << PlotStream::Clear()
		  << PlotStream::Left()
		  << PlotStream::Family("Sans Serif")
		  << PlotStream::Size(16)
		  << "― original \n"
		  << PlotStream::Color("darkRed")
		  << "― cubic spline \n"
		  << PlotStream::Color("darkBlue")
		  << "― cubic spline log \n"
		  << PlotStream::EndP();
  
  view.show();
  window.show();
  app.exec();
  return 1;
}


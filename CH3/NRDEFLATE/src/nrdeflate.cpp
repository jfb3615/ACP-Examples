#include "QatPlotWidgets/PlotView.h"
#include "QatPlotting/PlotStream.h"
#include "QatGenericFunctions/Variable.h"
#include "QatPlotting/PlotFunction1D.h"
#include "QatPlotting/PlotPoint.h"
#include "QatPlotting/PlotProfile.h"
#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <cstdlib>
#include <iostream>
#include <string>

double newtonRaphson(double x, Genfun::GENFUNCTION P) { 
  double x1=x;
  while (1) {
    double deltaX=-P(x)/P.prime()(x); 
    x+=deltaX;
    if (float(x1)==float(x)) break; 
    x1=x;
  }
  return x; 
}

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
  rect.setXmin(0.0);
  rect.setXmax(5.0);
  rect.setYmin(-2.0);
  rect.setYmax(2.0);
  

  PlotView view(rect);
  view.setXZero(false);
  view.setGrid(false);
  window.setCentralWidget(&view);
  
  using namespace Genfun;
  Variable X;
  GENFUNCTION F=(X-1)*(X-2)*(X-3)*(X-M_PI)*(X-4);
  PlotFunction1D P=F;
  view.add(&P);

  {
    PlotFunction1D::Properties prop;
    prop.pen.setWidth(3);
    //prop.pen.setColor("darkRed");
    P.setProperties(prop);
  }

  
  std::vector<std::string> title={"No roots found",
				  "One root found",
				  "Two roots found",
				  "Three roots found",
				  "Four roots found",
				  "Five roots found"};


  PlotStream titleStream(view.titleTextEdit());
  titleStream << PlotStream::Clear()
	      << PlotStream::Center() 
	      << PlotStream::Family("Sans Serif") 
	      << PlotStream::Size(16)
	      << title[0]
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
	       << "y=f(x)"
	       << PlotStream::EndP();
  
  view.show();
  window.show();
  app.exec();

  const AbsFunction * f=&F;
  PlotProfile prf;
  {
    PlotProfile::Properties prop;
    prop.brush.setStyle(Qt::SolidPattern);
    prop.brush.setColor("darkBlue");
    prop.symbolSize=10;
    prf.setProperties(prop);
  }
				  
  for (int i = 0; i<5; i++ ) {

    PlotStream titleStream(view.titleTextEdit());
    titleStream << PlotStream::Clear()
		<< PlotStream::Center() 
		<< PlotStream::Family("Sans Serif") 
		<< PlotStream::Size(16)
		<< title[i+1]
		<< PlotStream::EndP();
  

    double x = newtonRaphson(-1.0, *f);
    prf.addPoint(x,0);
    
    GENFUNCTION F1 = (*f)/(X-x);
    PlotFunction1D P1=F1;
    {
      PlotFunction1D::Properties prop;
      prop.pen.setWidth(2);
      prop.pen.setStyle(Qt::DashLine);
      P1.setProperties(prop);
    }
    view.add(&P);
    view.add(&P1);
    view.add(&prf);
    app.exec();
    view.clear();
    if (f!=&F) delete f;
    f=F1.clone();

  }

  return 1;
}


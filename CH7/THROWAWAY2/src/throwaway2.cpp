#include "QatDataAnalysis/Hist1D.h"
#include "QatDataAnalysis/Hist2D.h"
#include "QatPlotting/PlotHist1D.h"
#include "QatPlotting/PlotHist2D.h"
#include "QatPlotWidgets/PlotView.h"
#include "QatPlotting/PlotStream.h"
#include "QatGenericFunctions/Variable.h"
#include "QatGenericFunctions/Square.h"
#include "QatGenericFunctions/Cos.h"
#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <cstdlib>
#include <iostream>
#include <string>
#include <random>
#include <stdexcept>
typedef std::mt19937 EngineType;
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

  const double MAXTHROW=2.0;
  EngineType e;
  std::uniform_real_distribution<double> 
    rx(-1.0,1.0), 
    rphi(0.0,2*M_PI), 
    ry(0.0,MAXTHROW);

  Hist2D hxy("X vs PHI", 
	     25, -1.0, 1.0,
	     25,  0, 2.0*M_PI);
  

  Genfun::Variable    X(0,2),PHI(1,2);
  Genfun::Square      Square;
  Genfun::Cos         Cos;
  Genfun::GENFUNCTION f = 1/(2-X) + (2-X) - 2.0*(1-X*X)*Square(Cos(PHI));
	    
  for (int i=0;i<1000000;i++) {
    Genfun::Argument arg(2);
    arg[0]=rx(e);
    arg[1]=rphi(e);
    double y = ry(e);
    if (y<f(arg)) {
      hxy.accumulate(arg[0],arg[1]);
    }
    else if (y>MAXTHROW) {
      throw std::runtime_error ("Function value exceeds max random no");
    }
  }
  PlotHist2D pxy=hxy;
  {
    PlotHist2D::Properties prop;
    prop.brush.setStyle(Qt::SolidPattern);
    prop.pen.setWidth(0);
    prop.brush.setColor("lightgray");
    prop.pen.setWidth(0);
    prop.pen.setColor("darkgray");
    pxy.setProperties(prop);
  }
  PlotView view(pxy.rectHint());
  view.setGrid(false);
  view.setXZero(false);
  view.setYZero(false);
  view.xAxisFont().setPointSize(12);
  view.yAxisFont().setPointSize(12);

  view.add(&pxy);

  window.setCentralWidget(&view);
  
  PlotStream titleStream(view.titleTextEdit());
  titleStream << PlotStream::Clear()
	      << PlotStream::Center() 
	      << PlotStream::Family("Sans Serif") 
	      << PlotStream::Size(16)
	      << PlotStream::EndP();
  
  
  PlotStream xLabelStream(view.xLabelTextEdit());
  xLabelStream << PlotStream::Clear()
	       << PlotStream::Center()
	       << PlotStream::Family("Sans Serif")
	       << PlotStream::Size(16)
	       << "x=cos(θ)"
	       << PlotStream::EndP();
  
  PlotStream yLabelStream(view.yLabelTextEdit());
  yLabelStream << PlotStream::Clear()
	       << PlotStream::Center()
	       << PlotStream::Family("Sans Serif")
	       << PlotStream::Size(16)
	       << "φ"
	       << PlotStream::EndP();
  
  
  view.show();
  window.show();
  app.exec();

  
  return 1;
}


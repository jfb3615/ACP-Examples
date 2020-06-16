#include "QatGenericFunctions/Variable.h"
#include "QatGenericFunctions/RKIntegrator.h"
#include "QatGenericFunctions/FixedConstant.h"
#include "QatGenericFunctions/CubicSplinePolynomial.h"
#include "QatPlotWidgets/PlotView.h"
#include "QatPlotWidgets/PlotHist1DDialog.h"
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
  QAction  *nextAction=toolBar->addAction("Next");
  QAction  *editAction=toolBar->addAction("Edit");
  PlotHist1DDialog *editorDialog=new PlotHist1DDialog(NULL);
  
  nextAction->setShortcut(QKeySequence("n"));
  editAction->setShortcut(QKeySequence("e"));
  
  QObject::connect(nextAction, SIGNAL(triggered()), &app, SLOT(quit()));
  QObject::connect(editAction, SIGNAL(triggered()), editorDialog, SLOT(show()));
  
  PRectF rect;
  double MIN=999990;
  double MAX=1000000;
  rect.setXmin(MIN);
  rect.setXmax(MAX);
  rect.setYmin(-2.0);
  rect.setYmax(2.0);
  
  //
  // Harmonic oscillator:
  //
  double K=1.0; // Newtons/m
  double M=1.0; // Kg
  
  double eps =0.1;
  double p=1.0;
  double x=0.0;
  double t=0.0;
  CubicSplinePolynomial XInterp,PInterp;
  while (t<MAX) {
    t += eps;
    x += p/M*eps;
    p -= K*x*eps;
    if (t>MIN) {
      XInterp.addPoint(t,x);
      PInterp.addPoint(t,p);
    }
  }
  

  PlotView view(rect);
  window.setCentralWidget(&view);


  PlotFunction1D PlusOne=Genfun::FixedConstant(1.0);
  PlotFunction1D MnusOne=Genfun::FixedConstant(-1.0);
  PlotFunction1D PXInterp=XInterp;
  PlotFunction1D PPInterp=PInterp;

  PlotFunction1D::Properties prop;
  prop.pen.setWidth(3);
  prop.pen.setStyle(Qt::SolidLine);
  PXInterp.setProperties(prop);
  prop.pen.setStyle(Qt::DashLine);
  PPInterp.setProperties(prop);

  view.add(&PXInterp);
  view.add(&PPInterp);
  view.add(&PlusOne);
  view.add(&MnusOne);


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


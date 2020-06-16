#include "QatGenericFunctions/Variable.h"
#include "QatGenericFunctions/QuadratureRule.h"
#include "QatGenericFunctions/SimpleIntegrator.h"
#include "QatPlotWidgets/PlotView.h"
#include "QatPlotWidgets/CustomRangeDivider.h"
#include "QatPlotting/PlotStream.h"
#include "QatPlotting/PlotFunction1D.h"
#include "QatPlotting/PlotProfile.h"
#include "QatPlotting/PlotText.h"
#include "QatPlotting/PlotKey.h"
#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <cstdlib>
#include <iostream>
#include <string>
#include <cmath>
#include <memory> 
#include <libgen.h> // for dirname
//
// This program plots the convergence of an integral vs. the mesh spacing
// on a log-log plot, for a few integrands.

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
  rect.setXmin(0.5);
  rect.setXmax(1000);
  rect.setYmin(1E-7);
  rect.setYmax(10.0);

  CustomRangeDivider dvdY;
  QTextDocument txt;
  txt.setPlainText("10⁻⁵"); dvdY.add(1.0E-5, &txt);
  txt.setPlainText("10⁻³"); dvdY.add(1.0E-3, &txt);
  txt.setPlainText("0.1");  dvdY.add(1.0E-1, &txt);
  txt.setPlainText("10");   dvdY.add(10.0, &txt);
  
  
  PlotView view(rect);
  view.setYRangeDivider(&dvdY);
  view.setLogX(true);
  view.setLogY(true);
  view.setGrid(false);
  window.setCentralWidget(&view);

  view.yAxisFont().setPointSize(12.0);
  
  PlotStream statStream(view.statTextEdit());
  statStream << PlotStream::Clear()
	     << PlotStream::Center() 
	     << PlotStream::Family("Sans Serif") 
	     << PlotStream::Size(16);


  using namespace Genfun;
  Variable X;

  GENFUNCTION f0=1+X+3*X*X, f1=1+X+6*X*X, f2=1+X+6*X*X*X;
  const AbsFunction *f[]={&f0,&f1,&f2};
  const double anaAnswer[]={2.5,3.5, 3.0};   
  
  PlotProfile prof[3];

  std::string color[]={"black", "black", "black"};
  PlotProfile::Properties::SymbolStyle style[]={
    PlotProfile::Properties::CIRCLE,
    PlotProfile::Properties::SQUARE,
    PlotProfile::Properties::TRIANGLE_U};
    
  PlotProfile::Properties prop;
  TrapezoidRule           rule;
  
  
  prop.pen.setColor("red");
  prop.brush.setStyle(Qt::SolidPattern);
  prop.symbolSize=5;
  statStream << PlotStream::Clear();
  for (int p=0;p<3;p++) {
    prop.symbolStyle=style[p];
    prop.brush.setColor(color[p].c_str());
    prop.pen.setColor(color[p].c_str());
    prof[p].setProperties(prop);
    for (unsigned int e=0;e<10;e++) {
      unsigned int N = 1 << e;
      SimpleIntegrator        integrator(0,1,rule,N);
      double numAnswer =      integrator(*f[p]);
      double diff      =      numAnswer-anaAnswer[p];
      if (!finite(diff)) {
	break;
      }
      prof[p].addPoint(double(N),fabs(diff));
    }
  }
  statStream << PlotStream::EndP();
  view.add(&prof[0]);
  view.add(&prof[1]);
  view.add(&prof[2]);
  
  
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
	       << "Number of Intervals"
	       << PlotStream::EndP();
  
  PlotStream yLabelStream(view.yLabelTextEdit());
  yLabelStream << PlotStream::Clear()
	       << PlotStream::Family("Sans Serif")
	       << PlotStream::Center()
	       << PlotStream::Size(16)
	       << "δ"
	       << PlotStream::EndP();

  PlotKey  key(60,8);
  key.add(&prof[0],"1+x+3x²");
  key.add(&prof[1],"1+x+6x²");
  key.add(&prof[2],"1+x+6x³");
  view.add(&key);

  view.show();
  window.show();
  app.exec();
  return 1;
}


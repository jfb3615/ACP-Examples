#include "QatPlotWidgets/PlotView.h"
#include "QatPlotting/PlotStream.h"
#include "QatPlotting/PlotFunction1D.h"
#include "QatPlotting/PlotProfile.h"
#include "QatPlotting/RealArg.h"
#include "QatDataAnalysis/OptParse.h"
#include "QatGenericFunctions/TchebyshevPolynomial.h"
#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
using namespace Genfun;

// This program plots Tchebyshev polynomials and their roots, which
// are calcuated from a simple formula. 


std::vector<double>  rootsTchebyshev(unsigned int n, TchebyshevPolynomial::Type type) {
 std::vector< double > zero;
 if (n==0) {
   // nothing to do.  no zeros.
 }
 else if (n==1) {
   zero.push_back(0.0);
 }
 else {
   for (unsigned int k=0;k<n;k++) {
     unsigned int m=k+1;
     if (type==TchebyshevPolynomial::FirstKind) {
       zero.push_back(cos((2*m-1)*M_PI/2.0/n));
     }
     else if (type==TchebyshevPolynomial::SecondKind) {
       zero.push_back(cos(m*M_PI/(n+1)));
     }
   }
 }
 std::sort(zero.begin(),zero.end());
 return zero;
}


int main (int argc, char * * argv) {

  
  std::string usage= std::string("usage: ") + argv[0] + " N=val Type=type[1/2]"; 
  NumericInput input;
  input.declare("N", "Order ", 0);
  input.declare("Type", "Type (1 or 2) ", 1);
  try {
    input.optParse(argc, argv);
  }
  catch (std::exception &) {
  }

  if (argc!=1) {
    std::cout << usage << std::endl;
    exit(0);
  }
  
  
  unsigned int n =  (unsigned int) (0.5+input.getByName("N"));
  unsigned int type =  (unsigned int) (0.5+input.getByName("Type"));
  TchebyshevPolynomial::Type T=TchebyshevPolynomial::FirstKind;
  if (type==1) {
    T=TchebyshevPolynomial::FirstKind;
  }
  else if (type==2) {
    T=TchebyshevPolynomial::SecondKind;
  }
  else {
    throw std::runtime_error("Type must be 1 or 2");
  }

  QApplication     app(argc,argv);
  
  QMainWindow window;
  QToolBar *toolBar=window.addToolBar("Tools");
  QAction  *quitAction=toolBar->addAction("Quit");
  
  quitAction->setShortcut(QKeySequence("q"));
  
  QObject::connect(quitAction, SIGNAL(triggered()), &app, SLOT(quit()));
  
  PRectF rect;
  rect.setXmin(-1.0);
  rect.setXmax(1.0);
  rect.setYmin(-10.0);
  rect.setYmax(10.0);
  

  PlotView view(rect);
  window.setCentralWidget(&view);

  PlotFunction1D p(TchebyshevPolynomial(n,T,TWIDDLE),RealArg::Gt(-1.0) && RealArg::Lt(1.0));
  view.add(&p);
  //Derivative if uncommented...
  //PlotFunction1D q(TchebyshevPolynomial(n,T,TWIDDLE).prime(),RealArg::Gt(-1.0) && RealArg::Lt(1.0));
  //view.add(&q);
  
  PlotProfile prof;
  {
    PlotProfile::Properties prop;
    prop.brush.setStyle(Qt::SolidPattern);
    prop.brush.setColor("darkRed");
    prop.symbolSize=10;
    prop.errorBarSize=0;
    prof.setProperties(prop);
  }
  
  std::vector<double> roots=rootsTchebyshev(n,T);
  for (unsigned int i=0;i<roots.size();i++) {
    prof.addPoint(QPointF(roots[i],0),0);
  }
  view.add(&prof);

  std::ostringstream stream;
  stream << n;
  PlotStream titleStream(view.titleTextEdit());
  titleStream << PlotStream::Clear()
	      << PlotStream::Center() 
	      << PlotStream::Family("Sans Serif") 
	      << PlotStream::Size(16)
	      << "Tchebyshev Polynomial "
	      << PlotStream::Italic()
	      << (T==TchebyshevPolynomial::FirstKind ? "T":"U")
	      << PlotStream::Sub()
	      << stream.str()
	      << PlotStream::Normal()
	      << "(x)"
	      << PlotStream::EndP();
  
  
  PlotStream xLabelStream(view.xLabelTextEdit());
  xLabelStream << PlotStream::Clear()
	       << PlotStream::Center()
	       << PlotStream::Family("Sans Serif")
	       << PlotStream::Size(16)
	       << PlotStream::Italic()
	       << "x"
	       << PlotStream::EndP();
  
  PlotStream yLabelStream(view.yLabelTextEdit());
  yLabelStream << PlotStream::Clear()
	       << PlotStream::Center()
	       << PlotStream::Family("Sans Serif")
	       << PlotStream::Size(16)
	       << PlotStream::Italic()
	       << (T==TchebyshevPolynomial::FirstKind ? "T":"U")
	       << PlotStream::Sub()
	       << stream.str()
	       << PlotStream::Normal()
	       << "(x)"
	       << PlotStream::EndP();
  
  
  view.show();
  window.show();
  app.exec();
  return 1;
}


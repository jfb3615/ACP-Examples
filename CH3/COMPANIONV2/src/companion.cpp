#include "QatPlotWidgets/PlotView.h"
#include "QatPlotting/PlotStream.h"
#include "QatGenericFunctions/Polynomial.h"
#include "QatPlotting/PlotFunction1D.h"
#include "QatPlotting/PlotProfile.h"
#include "Eigen/Dense"
#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <cstdlib>
#include <iostream>
#include <string>

// This program finds the roots of a fourth order polynomial using the
// companion matrix. In addition it finds the first derivative, co-displays
// it with the original polynomial, and finds its roots--thereby also finding
// the extrema of the original function.  


int main (int argc, char * * argv) {

  QApplication     app(argc,argv);
  
  QMainWindow window;
  QToolBar *toolBar=window.addToolBar("Tools");
  QAction  *nextAction=toolBar->addAction("Next");
  
  nextAction->setShortcut(QKeySequence("n"));
  
  QObject::connect(nextAction, SIGNAL(triggered()), &app, SLOT(quit()));
  
  PRectF rect;
  rect.setXmin(-5.0);
  rect.setXmax(5.0);
  rect.setYmin(-10.0);
  rect.setYmax(10.0);
  

  PlotView view(rect);
  window.setCentralWidget(&view);
  
  using namespace Genfun;
  using namespace Eigen;
  using namespace std;
  
  // Here we define a polynomial through its coefficients C which
  const vector<double> C  ={1,0,-4, -0.5, -1}; // x⁴-4x²-½x-1
  Polynomial pPolynomial(C.begin(),C.end());
  std::vector<double> cDeriv=pPolynomial.coefficientsOfDerivative();
  Polynomial pPrime        (cDeriv.begin(), cDeriv.end());
  
  
  //================  We plot the polynomial         ==//
  PlotFunction1D P= pPolynomial;
  PlotFunction1D Q= pPolynomial.prime();
  PlotFunction1D R =pPrime; 
  view.add(&P);
  view.add(&Q);
  view.add(&R);
  
  {
    PlotFunction1D::Properties prop;
    prop.pen.setWidth(3);
    prop.pen.setStyle(Qt::DashLine);
    R.setProperties(prop);
    prop.pen.setStyle(Qt::SolidLine);
    prop.pen.setColor("darkRed");
    Q.setProperties(prop);
    prop.pen.setColor("darkBlue");
    P.setProperties(prop);
  }
  //====================================//

  // --------------------------------Label the plot--------------------------------//
  // The y-label is a bit involved sinced we want to print the      //
  // polynomial function in a vaguely human-readable format.  //
  // This uses a few of the string editing features.                       //
  //----------------------------------------------------------------------------------- //
  PlotStream titleStream(view.titleTextEdit());
  titleStream << PlotStream::Clear()
	      << PlotStream::Center() 
	      << PlotStream::Family("Sans Serif") 
	      << PlotStream::Size(16)
	      << (int) (C.size()-1)
	      << "th order polynomial example"
	      << PlotStream::EndP();
  
  
  PlotStream xLabelStream(view.xLabelTextEdit());
  xLabelStream << PlotStream::Clear()
	       << PlotStream::Center()
	       << PlotStream::Family("Sans Serif")
	       << PlotStream::Size(16)
	       << "x"
	       << PlotStream::EndP();

  // The "rather involved part of labeling"..
  PlotStream yLabelStream(view.yLabelTextEdit());
  yLabelStream << PlotStream::Clear()
	       << PlotStream::Center()
	       << PlotStream::Family("Sans Serif")
	       << PlotStream::Size(16);
  for (size_t i=C.size()-1;int(i)>=0;i--) {
    if (C[i]!=0) {
      if (i<C.size()-1) yLabelStream << C[i];
      if (i>0) {
	yLabelStream<< "x"; 
	if (i!=1) yLabelStream << PlotStream::Super() << int(i)    << PlotStream::Normal();
	bool nextPos=false;
        for (int j=i-1;j>0;j--) {
	  if (C[j]>0) {
	    nextPos=true;
	    break;
	  }
	}
	if (nextPos) yLabelStream << "+";
      }
    }
  }
  yLabelStream<< PlotStream::EndP();

  // Show the plot:
  view.show();
  window.show();
  app.exec();
  //
  // The plot will show once and after the user clicks "next" we will
  // construct the companion matrix and use it to find the roots.
  //
  PlotProfile prf, prq;
  {
    PlotProfile::Properties prop;
    prop.brush.setStyle(Qt::SolidPattern);
    prop.brush.setColor("darkBlue");
    prop.symbolSize=10;
    prf.setProperties(prop);
    prop.brush.setColor("darkRed");
    prq.setProperties(prop);
  }

  //
  // Plot the roots:
  //
  {
    std::vector<std::complex<double>> roots=pPolynomial.getRoots();
    for (size_t i=0;i<roots.size();i++) {
      if (roots[i].imag()==0) prf.addPoint(roots[i].real(),0);
    }
  }
  //
  // Plot the extrema:
  //
  {
    std::vector<std::complex<double>> roots=pPrime.getRoots();
    for (size_t i=0;i<roots.size();i++) {
      if (roots[i].imag()==0) prq.addPoint(roots[i].real(),pPolynomial(roots[i].real()));
    }
  }
  //
  // And pop the view one more time:
  //
  view.add(&prf);
  view.add(&prq);
  app.exec();
  view.clear();

  
  return 1;
}


#include "QatPlotWidgets/PlotView.h"
#include "QatPlotting/PlotStream.h"
#include "QatPlotting/PlotFunction1D.h"
#include "QatPlotting/PlotProfile.h"
#include "QatPlotting/RealArg.h"
#include "QatDataAnalysis/OptParse.h"
#include "QatGenericFunctions/Sigma.h"
#include "QatGenericFunctions/Sqrt.h"
#include "QatGenericFunctions/Cos.h"
#include "QatGenericFunctions/Exp.h"
#include "QatGenericFunctions/Power.h"
#include "QatGenericFunctions/Variable.h"
#include "QatGenericFunctions/AssociatedLaguerrePolynomial.h"
#include "Eigen/Dense"
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


// This program plots the associated laguerre polynomials together with
// their roots.  The roots are obtained from the three term recurrence
// relation, using eigenvalue methods.





#include "gsl/gsl_errno.h"
int main (int argc, char * * argv) {

  // Automatically generated:-------------------------:

  gsl_set_error_handler_off();
  std::string usage= std::string("usage: ") + argv[0] + " n=val a=val"; 
  NumericInput input;
  input.declare("n", "Degree ", 1);
  input.declare("a", "Order ",  1);
  try {
    input.optParse(argc, argv);
  }
  catch (std::exception &) {
  }

  if (argc!=1) {
    std::cout << usage << std::endl;
    exit(0);
  }
  unsigned int n =  (unsigned int) (0.5+input.getByName("n"));
  double       a =  input.getByName("a");

  QApplication     app(argc,argv);
  
  QMainWindow window;
  QToolBar *toolBar=window.addToolBar("Tools");
  QAction  *quitAction=toolBar->addAction("Quit");
  
  quitAction->setShortcut(QKeySequence("q"));
  
  QObject::connect(quitAction, SIGNAL(triggered()), &app, SLOT(quit()));
  
  PRectF rect;
  rect.setXmin(0.0);
  rect.setXmax(25.0);
  rect.setYmin(-10.0);
  rect.setYmax(10.0);
  

  PlotView view(rect);
  window.setCentralWidget(&view);

  PlotFunction1D p(AssociatedLaguerrePolynomial(n,a),RealArg::Gt(0.00));
  PlotFunction1D::Properties prop;
  prop.pen.setColor("darkRed");
  prop.pen.setWidth(3);
  p.setProperties(prop);
  view.add(&p);

  
  PlotProfile prof;
  {
    PlotProfile::Properties prop;
    prop.brush.setStyle(Qt::SolidPattern);
    prop.brush.setColor("darkRed");
    prop.symbolSize=10;
    prop.errorBarSize=0;
    prof.setProperties(prop);
  }


  Eigen::MatrixXd T=Eigen::MatrixXd::Zero(n,n);
  for (unsigned int i=0;i<n;i++) {
    for (unsigned int j=0;j<n;j++) {
      double an=-1/double(i+1);
      double bn=(2*i+a+1)/double(i+1);
      double cn=(i+a)/double(i+1);
      if (j==i-1) {
	T(i,j)=cn/an;
      }
      else if (j==i) {
	T(i,j)=-bn/an;
      }
      else if (j==i+1) {
	T(i,j)=1/an;
      }
    }
  } 
  Eigen::EigenSolver<Eigen::MatrixXd> solver(T);
  Eigen::VectorXcd eigenvalues=solver.eigenvalues();

  for (unsigned int i=0;i<eigenvalues.rows();i++) {
    prof.addPoint(QPointF(real(eigenvalues(i)),0),0);
  }
  view.add(&prof);
  
  std::ostringstream stream0, stream1;
  stream0 << n;
  stream1 << a;
  PlotStream titleStream(view.titleTextEdit());
  titleStream << PlotStream::Clear()
	      << PlotStream::Center() 
	      << PlotStream::Family("Sans Serif") 
	      << PlotStream::Size(16)
	      << "Associated Laguerre Polynomial "
	      << PlotStream::Italic()
	      << "L"
	      << PlotStream::Sub()
	      << stream0.str()
	      << PlotStream::Super()
	      << stream1.str()
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
	       << "L"
	       << PlotStream::Sub()
	       << stream0.str()
	       << PlotStream::Super()
	       << stream1.str()
	       << PlotStream::Normal()
	       << "(x)"
	       << PlotStream::EndP();
  
  
  view.show();
  window.show();
  app.exec();
  return 1;
}


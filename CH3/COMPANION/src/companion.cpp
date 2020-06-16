#include "QatPlotWidgets/PlotView.h"
#include "QatPlotting/PlotStream.h"
#include "QatGenericFunctions/Variable.h"
#include "QatGenericFunctions/FixedConstant.h"
#include "QatGenericFunctions/Sigma.h"
#include "QatGenericFunctions/Pi.h"
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
// companion matrix. Any real roots are co-displayed with the function.
// Then all of the roots--including the complex ones-- are printed out.


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
  // are held in a vector. This is done by writing p(x)=Σ cᵢ (Π₀ⁱ x)
  Variable X;
  const vector<double> C={-1,-0.5,-4,0, 1};
  // it must be monic============ ^
  
  Sigma poly;
  for (size_t i=0;i<C.size();i++) {
    Pi mono;
    mono.accumulate(C[i]*FixedConstant(1.0));
    for (size_t j=0;j<i;j++) mono.accumulate(X);
    poly.accumulate(mono);
  }
  //----------------------------------------------------------------------------------
		    
  //================  We plot the polynomial         ==//
  PlotFunction1D P=poly;
  view.add(&P);
  
  {
    PlotFunction1D::Properties prop;
    prop.pen.setWidth(3);
    prop.pen.setColor("darkRed");
    prop.pen.setStyle(Qt::DashLine);
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
  PlotProfile prf;
  {
    PlotProfile::Properties prop;
    prop.brush.setStyle(Qt::SolidPattern);
    prop.brush.setColor("darkBlue");
    prop.symbolSize=10;
    prf.setProperties(prop);
  }
  //
  // Construct the companion matrix:
  //
  MatrixXd comp=MatrixXd::Zero(C.size()-1,C.size()-1);
  for (int i=0;i<comp.rows();i++) {
    size_t lastCol=comp.cols()-1;
    comp(i,lastCol)=-C[i];
    if (i>0) comp(i,i-1)=1;
  }
  //
  // Print it out:
  //
  cout << endl;
  cout << "Companion matrix:\n" << comp << endl;
  cout << endl;
  //
  // Diagonalize it:
  //
  EigenSolver<MatrixXd> solver(comp);
  cout << "Roots of the equation:\n" << solver.eigenvalues() << endl;
  //
  // Now put the solution onto the graph:
  //
  for (int i=0;i<solver.eigenvalues().rows();i++) {
    if (solver.eigenvalues()(i).imag()==0) prf.addPoint(solver.eigenvalues()(i).real(),0);
  }
  //
  // And pop the view one more time:
  //
  view.add(&P);
  view.add(&prf);
  app.exec();
  view.clear();

  
  return 1;
}


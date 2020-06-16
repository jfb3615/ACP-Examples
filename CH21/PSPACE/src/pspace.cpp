#include "Eigen/Dense"
#include "QatPlotWidgets/PlotView.h"
#include "QatPlotWidgets/MultipleViewWindow.h"
#include "QatPlotWidgets/MultipleViewWidget.h"
#include "QatPlotWidgets/CustomRangeDivider.h"
#include "QatPlotting/PlotStream.h"
#include "QatPlotting/PlotFunction1D.h"
#include "QatPlotting/PlotProfile.h"
#include "QatGenericFunctions/FixedConstant.h"
#include "QatGenericFunctions/Variable.h"
#include "QatGenericFunctions/LegendrePolynomial.h"
#include "QatGenericFunctions/GaussQuadratureRule.h"
#include "QatGenericFunctions/GaussIntegrator.h"
#include "QatDataAnalysis/OptParse.h"
#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QTextEdit>
#include <QAction>
#include <cstdlib>
#include <iostream>
#include <string>
#include <sstream>
#include <thread>

using namespace Eigen;
using namespace Genfun;
using namespace std;

int main (int argc, char * * argv) {

  typedef unique_ptr<PlotFunction1D> PFPtr;

  // In this program, the units are MeV and fm.
  
  string usage= string("usage: ") + argv[0] + "[NGRID=val/100]";

  unsigned int NGRID=100;
  NumericInput input;
  input.declare("NGRID", "Number of gridpoints", NGRID);
  try {
    input.optParse(argc, argv);
  }
  catch (std::exception & e) {
    cerr << e.what() << endl;
    cerr << input.usage() << endl;
    cerr << usage << endl;
    exit (0);
  }

  NGRID=(unsigned int) (0.5+input.getByName("NGRID"));

  QApplication     app(argc,argv);
  
  MultipleViewWindow window;
  QToolBar *toolBar=window.addToolBar("Tools");
  QAction  *quitAction=toolBar->addAction("Quit");
  
  quitAction->setShortcut(QKeySequence("q"));
  
  QObject::connect(quitAction, SIGNAL(triggered()), &app, SLOT(quit()));

  GaussLegendreRule rule(NGRID);
  MatrixXd W=MatrixXd::Zero(NGRID,NGRID);
  MatrixXd WB2=MatrixXd::Zero(NGRID,NGRID);
  MatrixXd H=MatrixXd::Zero(NGRID,NGRID);

  Variable    P,XCOS;
  //
  // NOTE: In this example we use Gaussian Integration only 
  // for the angular integration; i.e., the computation of V_{ij}
  // We discretize on a regular grid with NGRID points; the
  // integration is equivalent to the midpoint method.
  // 
  for (unsigned int i=0;i<NGRID;i++) {
    double hi=(i+0.5)/NGRID;          // abscissas
    double gi=1.0/NGRID;              // weights
    double pi=hi/(1-hi);              // momenta
    double wi=pi*pi/pow(1-hi,2)*gi;   // w's
    
    W(i,i)=wi;
    WB2(i,i)=1/sqrt(wi);
    for (unsigned int j=0;j<NGRID;j++) {
      double dij= i==j ? 1 : 0;
      double hj=(j+0.5)/NGRID;        // abscissas
      double gj=1.0/NGRID;            // weights
      double qj=hj/(1-hj);            // momenta
      double wj=qj*qj/pow(1-hj,2)*gj; // w's
            
      GaussIntegrator integrator(rule);
      GENFUNCTION DP=pi*pi+qj*qj-2.0*pi*qj*XCOS;
      //
      // Assumes L=0;  
      //
      double Vij=-integrator(4*M_PI/DP)/4/M_PI/M_PI;
      H(i,j)=0.5*pi*pi*wi*dij + wi*Vij*wj;
    }
  }
  

  
  SelfAdjointEigenSolver<MatrixXd> solver(WB2*H*WB2);

  PlotProfile prof;
  {
    PlotProfile::Properties prop;
    prop.symbolSize=10;
    prop.brush.setStyle(Qt::SolidPattern);
    prop.brush.setColor("darkBlue");
    prof.setProperties(prop);
  }
  for (unsigned int i=0;i<4;i++) {
    prof.addPoint(0.5, solver.eigenvalues()(i));
  }
  
  
  PlotView viewEnergy;
  viewEnergy.setGrid(false);
  {
    PRectF rect;
    rect.setXmin(0);
    rect.setXmax(1);
    rect.setYmin(-0.51);
    rect.setYmax(0.01);
    viewEnergy.setRect(rect);
  }
  window.add(&viewEnergy, "Energy levels");
  viewEnergy.add(&prof);
  
  
  vector<PFPtr> plotFunctions;
  vector<PlotView *> scanView;
  vector<PFPtr> plotSolutions;

  
  // Hold on to pointers to functions:
  vector<PFPtr> plotAnal;


  for (int i=0;i<10;i++) {
    
    plotAnal.push_back(PFPtr(new PlotFunction1D(FixedConstant(-0.5/(i*i)))));
    
    PlotFunction1D::Properties prop;
    prop.pen.setColor("red");
    prop.pen.setWidth(1.0);
    plotAnal.back()->setProperties(prop);
    viewEnergy.add(plotAnal.back().get());
  }
  
  // Blank out the x-axis.    
  CustomRangeDivider divider;
  viewEnergy.setXRangeDivider(&divider);
  
  {
    PlotStream titleStream(viewEnergy.titleTextEdit());
    titleStream << PlotStream::Clear()
		<< PlotStream::Center() 
		<< PlotStream::Family("Sans Serif") 
		<< PlotStream::Size(14)
		<< "Hydrogen S=0 Energy levels, N"
		<< PlotStream::Sub()
		<< "points"
		<< PlotStream::Normal()
		<< "="
		<< (double) NGRID
		<< PlotStream::EndP();
    
    
    PlotStream yLabelStream(viewEnergy.yLabelTextEdit());
    yLabelStream << PlotStream::Clear()
		 << PlotStream::Center()
		 << PlotStream::Family("Sans Serif")
		 << PlotStream::Size(16)
		 << "Energy [Hartrees]"
		 << PlotStream::EndP();
    

  }



  window.show();
  app.exec();
  
  return 1;
}


#include "Psi2Hydrogen.h"
#include "QatGenericFunctions/AssociatedLaguerrePolynomial.h"
#include "QatGenericFunctions/Variable.h"
#include "QatGenericFunctions/Power.h"
#include "QatGenericFunctions/Exp.h"
#include "QatDataAnalysis/OptParse.h"
#include "QatDataAnalysis/Hist1D.h"
#include "QatPlotting/PlotHist1D.h"
#include "QatPlotting/PlotFunction1D.h"
#include "QatPlotting/PlotStream.h"
#include "QatPlotWidgets/PlotView.h"
#include "QatPlotWidgets/MultipleViewWindow.h"
#include "Eigen/Dense"
#include <QApplication>
#include <QToolBar>
#include <QAction>
#include <iostream>
#include <vector>
#include <sstream>
#include <iomanip>
#include <random>
using namespace std;
using namespace Genfun;

#define RBD_EPSILON 1E-4
class HydrogenAtomMarkovChain {


public:
  
  HydrogenAtomMarkovChain(int n, int l, int m, double sigma=1.0):psi(n,l,m),
								 gauss(0,sigma)
  {
  }

  double move(Eigen::VectorXd & v, double prob=0.0) {
    
    Eigen::VectorXd vProp=v;
    for (int d=0;d<vProp.rows();d++) {
      vProp[d]+=gauss(engine);
    }
    
    // The argument to the hydrogen wavefunction class is in spherical 
    // polar coordinates:
    Genfun::Argument r(3);
    r[0]=sqrt(vProp[0]*vProp[0]+vProp[1]*vProp[1]+vProp[2]*vProp[2]);
    r[1]=vProp[2]/r[0];
    r[2]=atan2(vProp[1],vProp[0]);
    
    
    double newProb=psi(r);
    if (newProb>prob) {
      v=vProp;
      return newProb;
    }
    else {
      if (flat(engine) < newProb/prob) {
	v=vProp;
	return newProb;
      }
      else {
	return prob;
      }
    }
  }
  
private:
  
  Genfun::Psi2Hydrogen psi;   // the Hydrogen PDF
  mt19937                                    engine;
  uniform_real_distribution<double>          flat;
  normal_distribution<double>                gauss;
};





int main (int argc, char ** argv) {


  std::string usage = std::string("usage: ") + argv[0] + "[NPOINTS=val{def=10000}] [N=val{def=0}] [L=val{def=0}] [M=val{def=0}] [fSigma=val{def=1.0}";
  NumericInput numeric;
  numeric.declare("NPOINTS", "Number of points in the Markov Chain", 10000);
  numeric.declare("N",       "Principal Quantum Number", 1);
  numeric.declare("L",       "Angular Momentum Quantum Number", 0);
  numeric.declare("M",       "Magnetic Quantum Number", 0);
  numeric.declare("fSigma",  "Under/over scale the proposal distribution", 1.0);
  numeric.declare("startingPoint",  "Starting Point", 0.0);
  numeric.optParse(argc, argv);
  
  if (argc>1) {
    std::cout << usage << std::endl;
    exit(0);
  }
  unsigned int NPOINTS = (unsigned int)(0.5+numeric.getByName("NPOINTS"));
  unsigned int N      = (unsigned int)(0.5+numeric.getByName("N"));
  unsigned int L      = (unsigned int)(0.5+numeric.getByName("L"));
  unsigned int M      = (unsigned int)(0.5+numeric.getByName("M"));
  double   fSigma     = numeric.getByName("fSigma");  
  double   startingPoint = numeric.getByName("startingPoint");

  QApplication app(argc,argv);


  
  Eigen::VectorXd v(3);
  v[0]=v[1]=v[2]=startingPoint;
  
  HydrogenAtomMarkovChain chain(N,L,M,N*fSigma);
  double prob=0;
  
  std::vector<Eigen::VectorXd> vColl;
  Hist1D HRad  ("Radial prob density",      200,   0, 20.0*N);
  Hist1D TRad  ("Radial time series",      1000, 0.0,   1000);
  Hist1D ARad  ("Radial autocorrelation",  1000, 0.0,   1000);
  for(unsigned int i=0;i<NPOINTS;i++) 
  {
    prob=chain.move(v,prob);
    HRad.accumulate(v.norm());
    vColl.push_back(v);
    TRad.accumulate(i+0.5,v.norm());
  }  
  HRad *= 1.0/(HRad.binWidth()*NPOINTS);
  PlotHist1D pHRad(HRad);
  {
    PlotHist1D::Properties prop;
    prop.plotStyle=PlotHist1DProperties::SYMBOLS;
    prop.pen.setWidth(3);
    prop.pen.setColor(QColor("darkBlue"));
    prop.brush.setColor(QColor("darkBlue"));
    prop.brush.setStyle(Qt::SolidPattern);
    pHRad.setProperties(prop);
  }


  Variable r;
  double asq = std::pow(2.0/N, 3.0)*tgamma(N-L-1+1)/(2.0*N*tgamma(N+L+1));
  GENFUNCTION ar = (2.0/N)*r;
  AssociatedLaguerrePolynomial Lg(N-L-1, 2*L+1);
  Exp exp;
  Power pow2L(2*L);
  
  // This is the radial distribution:
  GENFUNCTION f= r*r*asq*exp(-ar)*pow2L(ar)*Lg(ar)*Lg(ar);
  
  PlotFunction1D pF(f);
  {
    PlotFunction1D::Properties prop;
    prop.pen.setWidth(3);
    prop.pen.setColor(QColor("darkRed"));
    prop.brush.setStyle(Qt::SolidPattern);
    prop.brush.setColor(QColor("light cyan"));
    pF.setProperties(prop);
  }


  
  MultipleViewWindow window;
  QToolBar *toolBar=window.addToolBar("Tools");
  QAction  *quitAction=toolBar->addAction("Quit");
  
  quitAction->setShortcut(QKeySequence("q"));
  QObject::connect(quitAction, SIGNAL(triggered()), &app, SLOT(quit()));
  PlotView viewProb, viewTime, viewAuto;
  window.add(&viewProb,"Probability density");
  window.add(&viewTime,"Time series");
  window.add(&viewAuto,"Autocorrelation");

  viewProb.add(&pF);
  viewProb.add(&pHRad);
  viewProb.setRect(pHRad.rectHint());
  
  {
    PlotStream stream(viewProb.statTextEdit());
    stream << "\n";
    stream.stream << std::setprecision(3);
    stream << PlotStream::Size(12) << "<r/a₀>"  << HRad.mean() << "±";
    stream.stream << std::setprecision(2);
    stream << sqrt(HRad.variance()/HRad.entries()) <<  PlotStream::EndP();
  }
  {
    PlotStream stream(viewProb.xLabelTextEdit());
    stream << PlotStream::Center();
    stream << "<r/a" << PlotStream::Sub() << "0" << PlotStream::Normal() << ">";
    stream <<  PlotStream::EndP();
  }
  {
    PlotStream stream(viewProb.titleTextEdit());
    stream << PlotStream::Center();
    stream << PlotStream::Size(18);
    stream << "N=" ; stream.stream << N; 
    stream << " L="; stream.stream << L; 
    stream << " M="; stream.stream << M; 
    stream << PlotStream::EndP();
  }

  viewProb.setBox(true);
  viewProb.setGrid(false);
  viewProb.setXZero(false);
  viewProb.setYZero(false);
  viewProb.setLogX(false);
  viewProb.setLogY(false);

  viewTime.setBox(false);
  {
    PlotStream stream(viewTime.statTextEdit());
    stream << PlotStream::Clear();
  }
  {
    PlotStream stream(viewTime.yLabelTextEdit());
    stream << PlotStream::Clear();
    stream << PlotStream::Center();
    stream << "<r/a" << PlotStream::Sub() << "0" << PlotStream::Normal() << ">";
    stream <<  PlotStream::EndP();
  }
  {
    PlotStream stream(viewTime.xLabelTextEdit());
    stream << PlotStream::Clear();
    stream << PlotStream::Center();
    stream << "Iteration " << PlotStream::Italic() << "i" << PlotStream::Normal();
    stream <<  PlotStream::EndP();
  }
  PlotHist1D pTRad=TRad;
  viewTime.add(&pTRad);
  PRectF rect=pTRad.rectHint();
  rect.setYmin(0.001);
  
  viewTime.setRect(rect);
  viewTime.setLogY(true);

  
  double xixk=0, xixi=0, xkxk=0, xi=0, xk=0;
  unsigned int NA(0);
  for (unsigned int k=0;k<1000; k++) {
    for (unsigned int d=0;d<NPOINTS-1000;d+=1000) {
      if ( d+k < vColl.size()) {
	double x1=vColl[d].norm(),x2=vColl[d+k].norm();
	xi+=x1;
	xk+=x2;
	xixi+=x1*x1;
	xkxk+=x2*x2;
	xixk+=x1*x2;
	NA++;
      }
      else {
	break;
      }
    }
    if (N) {
      double A=(xixk/NA - xi/NA*xk/NA)/sqrt((xixi/NA - xi/NA*xi/NA)*(xkxk/NA - xk/NA*xk/NA));
      ARad.accumulate(k,A);
    }
  }

  viewAuto.setBox(false);
  viewAuto.setLogY(false);
  {
    PlotStream stream(viewAuto.statTextEdit());
    stream << PlotStream::Clear();
  }
  {
    PlotStream stream(viewAuto.yLabelTextEdit());
    stream << PlotStream::Clear();
    stream << PlotStream::Center();
    stream << "Autocorrelation r/a";
    stream <<  PlotStream::EndP();
  }
  {
    PlotStream stream(viewAuto.xLabelTextEdit());
    stream << PlotStream::Clear();
    stream << PlotStream::Center();
    stream << "Iteration " << PlotStream::Italic() << "i" << PlotStream::Normal();
    stream <<  PlotStream::EndP();
  }
  PlotHist1D pARad=ARad;
  viewAuto.add(&pARad);
  rect.setYmin(-1.0);
  rect.setYmax(1.0);
  viewAuto.setRect(rect);
  window.show();
  app.exec();

  return 0;
}






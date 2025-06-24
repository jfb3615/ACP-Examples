#include "QatPlotWidgets/PlotView.h"
#include "QatPlotting/PlotStream.h"
#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include "QatDataAnalysis/OptParse.h"
#include "QatDataModeling/ObjectiveFunction.h"
#include "QatDataModeling/MinuitMinimizer.h"
#include "QatGenericFunctions/Parameter.h"
#include "QatGenericFunctions/Variable.h"
#include "QatGenericFunctions/Exp.h"
#include "QatGenericFunctions/Sigma.h"
#include "QatPlotting/PlotFunction1D.h"
#include "QatGenericFunctions/GaussIntegrator.h"
#include "QatGenericFunctions/GaussQuadratureRule.h"
#include "Eigen/Dense"

// Follow the numbered steps to understand this program:
using namespace Eigen;
using namespace std;
using namespace Genfun;


//
// 1. We make an objective function:
//
class VariationalHydrogen:public ObjectiveFunction {

public:

  VariationalHydrogen(unsigned int N=4):_N(N),alpha(N),c(N) {
    for (unsigned int i=0;i<_N;i++) {
      {
	ostringstream stream;
	stream << "Alpha" << i; 
	//                       name          default    min  max
	alpha[i] = new Parameter(stream.str(), 1/(i+1.0), 0,   10);
      }
      {
	ostringstream stream;
	stream << "C" << i; 
	//                       name          default    min  max
	c[i] = new Parameter(stream.str(), 1/(i+1.0), 0,   10);
      }
    }
  }

  ~VariationalHydrogen () {
    for (unsigned int i=0;i<_N;i++) {
      delete alpha[i];
      delete c[i];
    }
  }
  Parameter * getAlpha(unsigned int i) const  { 
    return alpha[i];
  }    

  Parameter * getC(unsigned int i) const { 
    return c[i];
  }    

  unsigned int getNumComponents() const {
    return _N;
  }

  virtual double operator() () const {
    Eigen::MatrixXd S(_N,_N), T(_N,_N), A(_N,_N);
    Eigen::VectorXd C(_N);
    for (unsigned int i=0;i<_N;i++) {
      C(i) = i==0 ? 1: c[i]->getValue();
      for (unsigned int j=0;j<_N;j++) {
	S(i,j) = pow(M_PI/(alpha[i]->getValue()+alpha[j]->getValue()), 3.0/2.0);
	T(i,j) = 3.0*pow(M_PI,3.0/2.0)*alpha[i]->getValue()*alpha[j]->getValue()/pow(alpha[i]->getValue()+alpha[j]->getValue(),5.0/2.0);
	A(i,j) = -2*M_PI/(alpha[i]->getValue()+alpha[j]->getValue());
      }
    }
    return (C.transpose()*(T+A)*C)(0,0)/(C.transpose()*S*C)(0,0);
  }


private:
  
  unsigned int             _N;
  std::vector<Parameter *> alpha;
  std::vector<Parameter *> c;

  VariationalHydrogen(const VariationalHydrogen &);
  VariationalHydrogen & operator = (const VariationalHydrogen &);

};


int main (int argc, char * * argv) {

  // Automatically generated:-------------------------:

  std::string usage= std::string("usage: ") + argv[0] + "N=val"; 
  NumericInput input;
  input.declare("N", "Number of components", 4);
  try {
    input.optParse(argc, argv);
  }
  catch (exception &) {
    std::cout << usage         << std::endl;
    std::cout << input.usage() << std::endl;
    exit(0);
  }

  if (argc!=1) {
    std::cout << usage << std::endl;
  }

  unsigned int N=(unsigned int) (0.5+input.getByName("N"));

  QApplication     app(argc,argv);
  
  QMainWindow window;
  QToolBar *toolBar=window.addToolBar("Tools");
  QAction  *nextAction=toolBar->addAction("Next");
  
  nextAction->setShortcut(QKeySequence("n"));
  
  QObject::connect(nextAction, SIGNAL(triggered()), &app, SLOT(quit()));

  //
  // 2. Create a minimizer to minimize over the parameters:
  //
  bool verbose=true;
  VariationalHydrogen objective(N);
  MinuitMinimizer minimizer(verbose);
  minimizer.addStatistic(&objective);
  for (unsigned int i=0;i<objective.getNumComponents();i++) {
    minimizer.addParameter(objective.getAlpha(i));
    if (i!=0) minimizer.addParameter(objective.getC(i));
  }
  minimizer.minimize();
  
  //
  // 3.  Plot the result:
  //
  Variable    R;
  Sigma       F;
  vector<ConstTimesFunction> FSet; // Inelegance introduced for bkwds compat. 
  for (unsigned int i=0;i<objective.getNumComponents();i++) {
    ConstTimesFunction F0=objective.getC(i)->getValue()*Exp()(-objective.getAlpha(i)->getValue()*R*R);
    FSet.push_back(F0);
    F.accumulate(F0);
  }

  // G is the exact answer, for comparison:
  GENFUNCTION G=2*Exp()(-R);
  GaussLaguerreRule rule(10);
  GaussIntegrator  integrator(rule);
  double NORM=integrator(F);
  double NORM2=integrator(G);
  PRectF rect;
  PlotFunction1D p=F/NORM;
  PlotFunction1D q=G/NORM2;

  rect.setXmin(0.0);
  rect.setXmax(4.0);
  rect.setYmin(0.0);
  rect.setYmax(2.0);
  
  PlotView view(rect);
  vector<shared_ptr<PlotFunction1D>> PSet;
  for (unsigned int i=0;i<objective.getNumComponents();i++) {
    shared_ptr<PlotFunction1D> p0=shared_ptr<PlotFunction1D> (new PlotFunction1D(FSet[i]/NORM));
    PSet.push_back(p0);
    view.add(&*PSet.back());
  }


  view.add(&p);
  view.add(&q);
  PlotFunction1D::Properties prop;
  prop.pen.setWidth(4);
  prop.pen.setColor("darkRed");
  p.setProperties(prop);
  prop.pen.setColor("darkBlue");
  q.setProperties(prop);

  window.setCentralWidget(&view);
  
  PlotStream titleStream(view.titleTextEdit());
  titleStream << PlotStream::Clear()
	      << PlotStream::Center() 
	      << PlotStream::Family("Arial") 
	      << PlotStream::Size(12)
	      << "Hydrogen wavefunction, variational principle"
	      << PlotStream::EndP();
  
  
  PlotStream yLabelStream(view.yLabelTextEdit());
  yLabelStream << PlotStream::Clear()
	       << PlotStream::Center()
	       << PlotStream::Family("Arial")
	       << PlotStream::Size(16)
	       << PlotStream::Family("Symbol")
	       << "Y"
	       << PlotStream::Family("Arial")
	       << "(x)"
	       << PlotStream::EndP();
  
  PlotStream xLabelStream(view.xLabelTextEdit());
  xLabelStream << PlotStream::Clear()
	       << PlotStream::Center()
	       << PlotStream::Family("Arial")
	       << PlotStream::Size(16)
	       << "x"
	       << PlotStream::EndP();
  
 

  view.show();
  window.show();
  app.exec();
  return 1;
}


#include "DoublePendulumWidget.h"
#include "QatPlotting/PlotStream.h"
#include "QatPlotting/PlotFunction1D.h"
#include "QatGenericFunctions/Variable.h"
#include "QatGenericFunctions/Sin.h"
#include "QatGenericFunctions/Cos.h"
#include "QatGenericFunctions/Power.h"
#include "QatGenericFunctions/Parameter.h"
#include "QatGenericFunctions/RKIntegrator.h"
#include <iostream>
using namespace Genfun;
class DoublePendulumWidget::Clockwork {
public:

  RKIntegrator integrator;
  Parameter *mu=integrator.createControlParameter("mu", 1.0, 0.2, 5.0);
  Parameter *lambda=integrator.createControlParameter("lambda", 1.0, 0.2, 5.0);

  const Variable THETA1=Variable(0,4);
  const Variable THETA2=Variable(1,4);
  const Variable P1=Variable(2,4);
  const Variable P2=Variable(3,4);
  AbsFunction    *H = ( (P1*P1+(1+*mu)**lambda*P2*P2 -2**lambda*P1*P2*Cos()(THETA1-THETA2)) / 
			((2**lambda**lambda)*(*mu+Power(2)(Sin()(THETA1-THETA2)))) - (*mu+1)**lambda*Cos()(THETA1) - Cos()(THETA2)).clone();
  AbsFunction    *DTHETA1DT=H->partial(P1.index()).clone();
  AbsFunction    *DTHETA2DT=H->partial(P2.index()).clone();
  AbsFunction    *DP1DT=(-H->partial(THETA1.index())).clone();
  AbsFunction    *DP2DT=(-H->partial(THETA2.index())).clone();
  
  
  Parameter *theta1Start=NULL;
  Parameter *theta2Start=NULL;
  Parameter *p1Start    =NULL;
  Parameter *p2Start    =NULL;
  
  const AbsFunction *theta1Eqn=NULL;
  const AbsFunction *theta2Eqn=NULL;
  const AbsFunction *p1Eqn    =NULL;
  const AbsFunction *p2Eqn    =NULL;

  PlotFunction1D    *ptheta1Eqn=NULL;
  PlotFunction1D    *ptheta2Eqn=NULL;
  PlotFunction1D    *pp1Eqn=NULL;
  PlotFunction1D    *pp2Eqn=NULL;
};

DoublePendulumWidget::DoublePendulumWidget (QWidget *parent):QMainWindow(parent),c(new Clockwork()){
  ui.setupUi(this);
  // Label the plot.
  {
    PlotStream stream(ui.view->titleTextEdit());
    stream << PlotStream::Clear()
	       << PlotStream::Center() 
	       << PlotStream::Family("Sans Serif") 
	       << PlotStream::Size(16)
	       << "The Double Pendulum"
	       << PlotStream::EndP();

  }
  {
    PlotStream stream(ui.view->xLabelTextEdit());
    stream << PlotStream::Clear()
	       << PlotStream::Center() 
	       << PlotStream::Family("Sans Serif") 
	       << PlotStream::Size(16)
	       << "t"
	       << PlotStream::EndP();

  }
  // Connect signals to slots:
  ui.actionQuit->setShortcut(QKeySequence("q"));
  QObject::connect(ui.actionQuit, SIGNAL(triggered()), this, SLOT(quit()));
  QObject::connect(ui.p1SpinBox, SIGNAL(valueChanged(double)), this, SLOT(changeParameter(double)));
  QObject::connect(ui.p2SpinBox, SIGNAL(valueChanged(double)), this, SLOT(changeParameter(double)));
  QObject::connect(ui.muSpinBox, SIGNAL(valueChanged(double)), this, SLOT(changeParameter(double)));
  QObject::connect(ui.lambdaSpinBox, SIGNAL(valueChanged(double)), this, SLOT(changeParameter(double)));
  
  // Now set up the equations of motion:
  c->theta1Start=c->integrator.addDiffEquation(c->DTHETA1DT, "THETA1", 0, -5, 5);
  c->theta2Start=c->integrator.addDiffEquation(c->DTHETA2DT, "THETA2", 0, -5, 5);
  c->p1Start=c->integrator.addDiffEquation(c->DP1DT, "P1", 1, -5, 5);
  c->p2Start=c->integrator.addDiffEquation(c->DP2DT, "P2", -1, -5, 5);
  
  c->theta1Eqn=c->integrator.getFunction(c->THETA1.index());
  c->theta2Eqn=c->integrator.getFunction(c->THETA2.index());
  c->p1Eqn=c->integrator.getFunction(c->P1.index());
  c->p2Eqn=c->integrator.getFunction(c->P2.index());
  
  c->ptheta1Eqn=new PlotFunction1D(*c->theta1Eqn);
  c->ptheta2Eqn=new PlotFunction1D(*c->theta2Eqn);
  c->pp1Eqn=new PlotFunction1D(*c->p1Eqn);
  c->pp2Eqn=new PlotFunction1D(*c->p2Eqn);
  
  PRectF rect;
  rect.setXmin(0.0);
  rect.setXmax(20.0);
  rect.setYmin(-10.0);
  rect.setYmax(+10.0);
  ui.view->setRect(rect);
  ui.view->add(c->ptheta1Eqn);
  ui.view->add(c->ptheta2Eqn);
  ui.view->add(c->pp1Eqn);
  ui.view->add(c->pp2Eqn);

  PlotFunction1D::Properties prop;
  prop.pen.setWidth(3);
  prop.pen.setColor("darkBlue");
  c->ptheta1Eqn->setProperties(prop);
  prop.pen.setColor("darkRed");
  c->ptheta2Eqn->setProperties(prop);
  prop.pen.setColor("darkGreen");
  c->pp1Eqn->setProperties(prop);
  prop.pen.setColor("black");
  c->pp2Eqn->setProperties(prop);

  c->p1Start->setValue(ui.p1SpinBox->value());
  c->p2Start->setValue(ui.p2SpinBox->value());
  c->lambda->setValue(ui.lambdaSpinBox->value());
  c->mu->setValue(ui.muSpinBox->value());

}

DoublePendulumWidget::~DoublePendulumWidget() {
  delete c->H;
  delete c;
}

void DoublePendulumWidget::changeParameter(double x) {
  QObject *sender =QObject::sender();
  if (sender==ui.p1SpinBox) {
    c->p1Start->setValue(x);
  }
  else if (sender==ui.p2SpinBox) {
    c->p2Start->setValue(x);
  }
  else if (sender==ui.lambdaSpinBox) {
    c->lambda->setValue(x);
  }
  else if (sender==ui.muSpinBox) {
    c->mu->setValue(x);
  }
  ui.view->recreate();
}

void DoublePendulumWidget::refresh() {
  std::cout << "Called back to refresh" << std::endl;
}

void DoublePendulumWidget::quit(){
  exit (0);
}


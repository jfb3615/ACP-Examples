#include "QatPlotWidgets/PlotView.h"
#include "QatPlotWidgets/PlotHist1DDialog.h"
#include "QatPlotting/PlotStream.h"
#include "QatPlotting/PlotFunction1D.h"
#include "QatPlotting/PlotPoint.h"
#include "QatPlotting/RealArg.h"
#include "QatGenericFunctions/Exp.h"
#include "QatGenericFunctions/FixedConstant.h"
#include "QatGenericFunctions/Sqrt.h"
#include "QatGenericFunctions/Variable.h"
#include "QatGenericFunctions/RKIntegrator.h"
#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <cstdlib>
#include <iostream>
#include <string>
#include <unistd.h>

// This class catches signals that are emitted when the user
// hits the "next" button.  It increases the energy parameter
// and redraws the plot.
class Catcher : public QObject {

  Q_OBJECT

public:
  
  Catcher(PlotView *view, Genfun::Parameter & E):view(view),E(E){}
  
public slots:
  
  void update() {

    E.setValue(E.getValue()+0.01);
    PlotStream statStream(view->statTextEdit());
    
    statStream << PlotStream::Clear()
	       << PlotStream::Center()
	       << PlotStream::Family("Sans Serif")
	       << PlotStream::Size(16)
    	       << "E=" <<  E.getValue()
    	       << PlotStream::EndP();

    view->recreate();
  }

private:

  PlotView          * view;
  Genfun::Parameter & E; 
};




int main (int argc, char * * argv) {


  // Setup application, window, toolbar, next & quit buttons
  QApplication     app(argc,argv);
  
  QMainWindow window;
  QToolBar *toolBar=window.addToolBar("Tools");
  QAction  *nextAction=toolBar->addAction("Next");
  QAction  *quitAction=toolBar->addAction("Quit");
  nextAction->setShortcut(QKeySequence("n"));
  quitAction->setShortcut(QKeySequence("q"));

  // Make a plot view, put it in the window.
  PlotView view({0,20,-200,200});
  window.setCentralWidget(&view);
  
  // Set up the integrator and obtain a control parameter:
  Genfun::RKIntegrator  integrator0;
  Genfun::Parameter &E= *integrator0.createControlParameter("E", -4, -5, 5);

  // The signal catcher for plot update:
  Catcher catcher(&view,E);
  
  QObject::connect(quitAction, &QAction::triggered, &app,     &QApplication::quit);
  QObject::connect(nextAction, &QAction::triggered, &catcher, &Catcher::update);
  
  

  PlotStream(view.titleTextEdit())  << PlotStream::Size(12) << "(spinless) Deuteron: potential and wavefunction" << PlotStream::EndP();
  PlotStream(view.xLabelTextEdit()) << PlotStream::Size(16) << "x" << PlotStream::EndP();
  PlotStream(view.yLabelTextEdit()) << PlotStream::Size(16) << "ψ(x)" << PlotStream::EndP();
  PlotStream (view.statTextEdit())  << PlotStream::Clear()  << PlotStream::Size(16) << "E=" << E.getValue() << PlotStream::EndP();
  E.setValue(E.getValue()+0.01);

  
  // Numerical part:
  using namespace Genfun;
  
  // Here we define the internucleon potential, the Mafliet-Tjon
  // Potential:
  static const double hbarc=197.3; // MeV fm
  static const double mp   =938.3; // MeV
  static const double mn   =939.6; // MeV
  static const double mu   = mn*mp/(mn+mp);
  Exp exp;
  Variable R;
  GENFUNCTION V=-635*exp(-1.55*R)/(R+0.01) + 1458*exp(-3.11*R)/(R+0.01);
  
  
  // Set up variables for numerical integration and their diffierential
  // equations:
  Variable u(0,3),v(1,3),x(2,3);
  GENFUNCTION dudx=v;
  GENFUNCTION dvdx=2*mu/hbarc/hbarc*(V(x)-E)*u;
  GENFUNCTION dxdx=FixedConstant(1.0)%FixedConstant(1.0)%FixedConstant(1.0);
  
  // Add the differential equations to the integrator:
  integrator0.addDiffEquation(&dudx,"U",  1.0);
  integrator0.addDiffEquation(&dvdx,"V",  0.0);
  integrator0.addDiffEquation(&dxdx,"X",  0.0);
  
  // Get the solution to the differential equation, make plots from that:
  GENFUNCTION sU0=*integrator0.getFunction(u);
  PlotFunction1D pU0=0.25*sU0;
  PlotFunction1D pV =V;
  PlotFunction1D pE=E*FixedConstant(1.0);
  
  // Plot cosmetics:
  {
    PlotFunction1D::Properties prop;
    prop.pen.setWidth(3);
    pE.setProperties(prop);
    prop.pen.setColor("darkRed");
    pU0.setProperties(prop);
    prop.pen.setColor("darkBlue");
    pV.setProperties(prop);
  }

  // Put the plots in the Plot View:
  view.add(&pU0);
  view.add(&pV);
  view.add(&pE);
  
  // Interact:
  view.show();
  window.show();
  app.exec();
  return 0;
}

#include "theDeuteron.moc"



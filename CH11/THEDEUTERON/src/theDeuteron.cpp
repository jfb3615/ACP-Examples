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
#include <thread>
#include <unistd.h>

Genfun::RKIntegrator  integrator0;
Genfun::Parameter &E= *integrator0.createControlParameter("E", -4, -5, 5);


PlotView *view{nullptr};
class Catcher : public QObject {

  Q_OBJECT

public slots:
  void quit() { exit(0);}
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

};



void updateView() {
}

int main (int argc, char * * argv) {

  Catcher catcher;



  std::string usage= std::string("usage: ") + argv[0]; 
  if (argc!=1) {
    std::cout << usage << std::endl;
  }


  QApplication     app(argc,argv);
  
  QMainWindow window;
  QToolBar *toolBar=window.addToolBar("Tools");
  QAction  *nextAction=toolBar->addAction("Next");
  QAction  *quitAction=toolBar->addAction("Quit");
  
  nextAction->setShortcut(QKeySequence("n"));
  quitAction->setShortcut(QKeySequence("q"));
  
  QObject::connect(quitAction, SIGNAL(triggered()), &app, SLOT(quit()));
  QObject::connect(nextAction, SIGNAL(triggered()), &catcher, SLOT(update()));
  
  PRectF rect(0,20,-200,200);
  view = new PlotView(rect);
  window.setCentralWidget(view);

  PlotStream titleStream(view->titleTextEdit());
  titleStream << PlotStream::Clear()
	      << PlotStream::Center() 
	      << PlotStream::Family("Sans Serif") 
	      << PlotStream::Size(12)
	      << "(spinless) Deuteron: potential and wavefunction"
	      << PlotStream::EndP();
  
  
  PlotStream xLabelStream(view->xLabelTextEdit());
  xLabelStream << PlotStream::Clear()
	       << PlotStream::Center()
	       << PlotStream::Family("Symbol")
	       << PlotStream::Size(16)
	       << "x"
	       << PlotStream::EndP();
  
  PlotStream yLabelStream(view->yLabelTextEdit());
  yLabelStream << PlotStream::Clear()
	       << PlotStream::Center()
	       << PlotStream::Family("Symbol")
	       << PlotStream::Size(16)
	       << "y(x)"
	       << PlotStream::EndP();
  
  PlotStream statStream(view->statTextEdit());
  statStream << PlotStream::Clear()
	     << PlotStream::Center()
    	     << PlotStream::Family("Sans Serif")
	     << PlotStream::Size(16)
	     << "E="
	     << E.getValue()
	     << PlotStream::EndP();

 

  E.setValue(E.getValue()+0.01);

  
  // Numerical part:
  using namespace Genfun;
  
  
  static const double hbarc=197.3; // MeV fm
  static const double mp   =938.3; // MeV
  static const double mn   =939.6; // MeV
  static const double mu   = mn*mp/(mn+mp);
  Exp exp;
  Variable R;
  GENFUNCTION V=-635*exp(-1.55*R)/(R+0.01) + 1458*exp(-3.11*R)/(R+0.01);
  
  
  
  Variable u(0,3),v(1,3),x(2,3);
  GENFUNCTION dudx=v;
  GENFUNCTION dvdx=2*mu/hbarc/hbarc*(V(x)-E)*u;
  GENFUNCTION dxdx=FixedConstant(1.0)%FixedConstant(1.0)%FixedConstant(1.0);
  
  
  integrator0.addDiffEquation(&dudx,"U",  1.0);
  integrator0.addDiffEquation(&dvdx,"V",  0.0);
  integrator0.addDiffEquation(&dxdx,"X",  0.0);
  

  GENFUNCTION sU0=*integrator0.getFunction(u);
  Variable X;
  PlotFunction1D pU0=0.25*sU0;
  PlotFunction1D pV =V;
  {
    PlotFunction1D::Properties prop;
    prop.pen.setWidth(3);
    prop.pen.setColor("darkRed");
    pU0.setProperties(prop);
    prop.pen.setColor("darkBlue");
    pV.setProperties(prop);
  }

  PlotFunction1D pE=FixedConstant(E.getValue());
  view->add(&pU0);
  view->add(&pV);
  view->add(&pE);
  

  view->show();
  window.show();
  app.exec();
  return 0;
}

#include "theDeuteron.moc"



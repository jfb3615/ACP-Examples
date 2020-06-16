#include "QatGenericFunctions/CubicSplinePolynomial.h"
#include "QatPlotWidgets/PlotView.h"
#include "QatPlotWidgets/PlotHist1DDialog.h"
#include "QatPlotting/PlotStream.h"
#include "QatPlotting/PlotKey.h"
#include "QatPlotting/PlotFunction1D.h"
#include "QatGenericFunctions/RKIntegrator.h"
#include "QatGenericFunctions/Variable.h"
#include "QatGenericFunctions/Sqrt.h"
#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
int main (int argc, char * * argv) {

  // Automatically generated:-------------------------:

  std::string usage= std::string("usage: ") + argv[0]; 
  if (argc!=1) {
    std::cout << usage << std::endl;
  }


  QApplication     app(argc,argv);
  
  QMainWindow window;
  QToolBar *toolBar=window.addToolBar("Tools");
  QAction  *quitAction=toolBar->addAction("Quit");
  
  quitAction->setShortcut(QKeySequence("q"));
  
  QObject::connect(quitAction, SIGNAL(triggered()), &app, SLOT(quit()));
  
  PRectF rect;
  rect.setXmin(0.0);
  rect.setXmax(500.0);
  rect.setYmin(0.0);
  rect.setYmax(250.0);

  double m    =0.5; // kg
  double g    =9.8; // m/s/s
  
 
  using namespace Genfun;
  RKIntegrator integrator;
  Parameter & gamma=*integrator.createControlParameter("gamma", 0.0, 0.0, 10.0);
  Variable X(0,4),Y(1,4),VX(2,4),VY(3,4);

  GENFUNCTION DXDT  = VX;
  GENFUNCTION DYDT  = VY;
  GENFUNCTION DVXDT = -gamma/m*VX*Sqrt()(VX*VX+VY*VY);
  GENFUNCTION DVYDT = -g -gamma/m*VY*Sqrt()(VX*VX+VY*VY);
  

  integrator.addDiffEquation(&DXDT,  "X0", 0, -10, 10);
  integrator.addDiffEquation(&DYDT,  "Y0", 0, -10, 10);
  integrator.addDiffEquation(&DVXDT, "VX0", 50, -100, 100);
  integrator.addDiffEquation(&DVYDT, "VY0", 50, -100, 100);
  
  GENFUNCTION   x = *integrator.getFunction(X);
  GENFUNCTION   y = *integrator.getFunction(Y);

  // This is somewhat unusual in that we are plotting the orbit
  // here, not the functions themselves:
  const unsigned int NSET=5;
  Genfun::CubicSplinePolynomial cSpline[NSET];
  std::string label[NSET];
  for (unsigned int s=0;s<NSET;s++) {
    for (int t=0;t<10;t++) {
      cSpline[s].addPoint(x(t),y(t));
    }
    {
      std::ostringstream stream;
      stream << gamma.getValue();
      label[s]=stream.str();
    }
    gamma.setValue(gamma.getValue()+0.001);
  }


  PlotView view(rect);
  view.setBox(true);
  window.setCentralWidget(&view);
  view.setGrid(false);
  view.setXZero(false);
  view.setYZero(false);
  view.setBox(false);
  
  QColor colors[]={"black","dimGray", "black", "dimGray", "black"};
  Qt::PenStyle style[]={Qt::SolidLine, Qt::DashLine, Qt::DotLine, Qt::DashDotLine, Qt::DashDotDotLine};
  unsigned int CSIZE=sizeof(colors)/sizeof(QColor);
  std::vector<PlotFunction1D> pF(cSpline,cSpline+sizeof(cSpline)/sizeof(CubicSplinePolynomial));

  PlotKey key(300,240);
  QFont font;
  font.setPointSize(16);
  key.setFont(font);
  
  for (unsigned int i=0;i<pF.size();i++) {
    PlotFunction1D::Properties prop;
    prop.pen.setWidth(3);
    prop.pen.setColor(colors[i%CSIZE]);
    prop.pen.setStyle(style[i%CSIZE]);
    pF[i].setProperties(prop);
    view.add(&pF[i]);
    key.add(&pF[i],"γ="+label[i]);
  }
  view.add(&key);
  
  PlotStream titleStream(view.titleTextEdit());
  titleStream << PlotStream::Clear()
	      << PlotStream::Center() 
	      << PlotStream::Family("Sans Serif") 
	      << PlotStream::Size(16)
	      << "Projectile motion with air resistance"
	      << PlotStream::EndP();
  
  
  PlotStream xLabelStream(view.xLabelTextEdit());
  xLabelStream << PlotStream::Clear()
	       << PlotStream::Center()
	       << PlotStream::Family("Sans Serif")
	       << PlotStream::Size(16)
	       << "Position along x [m]"
	       << PlotStream::EndP();
  
  PlotStream yLabelStream(view.yLabelTextEdit());
  yLabelStream << PlotStream::Clear()
	       << PlotStream::Center()
	       << PlotStream::Family("Sans Serif")
	       << PlotStream::Size(16)
	       << "Position along y [m]"
	       << PlotStream::EndP();


  
  window.show();
  app.exec();
  return 1;
}


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
  QObject::connect(quitAction, &QAction::triggered, &app, &QApplication::quit);
  
  double m    =0.5; // kg
  double g    =9.8; // m/s/s
  
  // Set up an integrator. Request a control parameter, gamma. 
  using namespace Genfun;
  RKIntegrator integrator;
  Parameter & gamma=*integrator.createControlParameter("gamma", 0.0, 0.0, 10.0);

  // Here are coupled differential equations to be solved:
  Sqrt sqrt;
  Variable X(0,4),Y(1,4),VX(2,4),VY(3,4);
  GENFUNCTION DXDT  = VX;
  GENFUNCTION DYDT  = VY;
  GENFUNCTION DVXDT = -gamma/m*VX*sqrt(VX*VX+VY*VY);
  GENFUNCTION DVYDT = -g -gamma/m*VY*sqrt(VX*VX+VY*VY);
  
  // Add them to the integrator.  Set the initial values: 
  integrator.addDiffEquation(&DXDT,  "X0", 0, -10, 10);
  integrator.addDiffEquation(&DYDT,  "Y0", 0, -10, 10);
  integrator.addDiffEquation(&DVXDT, "VX0", 50, -100, 100);
  integrator.addDiffEquation(&DVYDT, "VY0", 50, -100, 100);

  // Here is the result.  
  GENFUNCTION   x = *integrator.getFunction(X);
  GENFUNCTION   y = *integrator.getFunction(Y);

  // This is somewhat unusual in that we are plotting the orbit y(x)
  // here, not the functions x(t) and y(t).  Use a cubic spline.
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

  // Set the view and some of its properteis;
  PlotView view({0.,500.,0.,250.});
  view.setBox(true);
  window.setCentralWidget(&view);
  view.setGrid(false);
  view.setXZero(false);
  view.setYZero(false);
  view.setBox(false);
  
  std::vector<QColor>       colors{"black",     "dimGray",     "black",     "dimGray",       "black"};
  std::vector<Qt::PenStyle> style{Qt::SolidLine, Qt::DashLine, Qt::DotLine, Qt::DashDotLine, Qt::DashDotDotLine};
  unsigned int CSIZE=colors.size();
  std::vector<PlotFunction1D> pF(cSpline,cSpline+NSET);
  
  // Create the key:
  PlotKey key(300,240);
  QFont font;
  font.setPointSize(16);
  key.setFont(font);

  // Add curves to the plot and to the key:
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
  
  // Labeling
  auto sz16=PlotStream::Size(16);
  PlotStream (view.titleTextEdit())  << sz16 << "Projectile motion with air resistance" << PlotStream::EndP();
  PlotStream (view.xLabelTextEdit()) << sz16 << "Position along x [m]" << PlotStream::EndP();
  PlotStream (view.yLabelTextEdit()) << sz16 << "Position along y [m]" << PlotStream::EndP();
  
  window.show();
  app.exec();
  return 1;
}


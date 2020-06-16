#include "QatPlotWidgets/MultipleViewWindow.h"
#include "QatPlotWidgets/PlotView.h"
#include "QatPlotting/PlotStream.h"
#include "QatPlotting/PlotProfile.h"
#include "QatPlotting/PlotFunction1D.h"
#include "QatPlotting/PlotErrorEllipse.h"
#include "QatPlotting/PlotPoint.h"
#include "QatPlotting/PlotKey.h"
#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <cstdlib>
#include <iostream>
#include <string>
#include <random>

#include "QatDataModeling/MinuitMinimizer.h"
#include "QatDataModeling/ChiSq.h"
#include "QatDataAnalysis/OptParse.h"
#include "QatGenericFunctions/Variable.h"
#include "QatGenericFunctions/Parameter.h"

int main (int argc, char * * argv) {

  //
  using namespace std;
  random_device dev;
  mt19937 engine(dev());
  double sigma=5.0;
  
  // Automatically generated:-------------------------:
  
  std::string usage= std::string("usage: ") + argv[0] + " [SIGMA=val/5.0]"; 
  NumericInput input;
  input.declare("SIGMA", "Assumed error on velocity measurement", sigma);

  try {
    input.optParse(argc, argv); 
  }
  catch (const std::exception & e) {
    std::cerr << e.what() << std::endl;
    std::cerr << input.usage() << std::endl;
    std::cerr << usage << std::endl;
    exit(0);
  }
  
  if (argc!=1) {
    std::cout << usage << std::endl;
  }

  sigma=input.getByName("SIGMA"); 
  normal_distribution<double> gauss(0,sigma);

  QApplication     app(argc,argv);
  
  MultipleViewWindow window;
  QToolBar *toolBar=window.addToolBar("Tools");
  QAction  *quitAction=toolBar->addAction("Quit");
  
  quitAction->setShortcut(QKeySequence("q"));
  
  QObject::connect(quitAction, SIGNAL(triggered()), &app, SLOT(quit()));
  
  PRectF rect;
  rect.setXmin(-4.0);
  rect.setXmax(12.0);
  rect.setYmin(-50.0);
  rect.setYmax(150.0);

  double v0=15.0;
  PlotProfile pf;
  ChiSq chi2;
  {
    PlotProfile::Properties prop;
    prop.symbolSize=6;
    prop.brush.setStyle(Qt::SolidPattern);
    //    prop.brush.setColor("darkRed");
    //prop.pen.setColor("darkRed");
    pf.setProperties(prop);
  }
  // Make measurements
  for (int t=0;t<10;t++) {
    double v=9.8*t+v0+gauss(engine);
    pf.addPoint(t,v, sigma);  // add to the profile plot
    chi2.addPoint(t,v,sigma); // add to the chisq objective
  }
    


  using namespace Genfun;
  Variable T;
  Parameter pv0("V0",0.0,  -100,   100);
  Parameter pg("g",  10.0, 5.0, 15.0);
  GENFUNCTION F=pv0+pg*T;

  const bool verbose=true;
  MinuitMinimizer minimizer(verbose);
  minimizer.addParameter(&pv0);
  minimizer.addParameter(&pg);
  minimizer.addStatistic(&chi2,&F);
  minimizer.minimize();

  PlotView view1(rect);
  rect.setXmin(0.0);
  rect.setXmax(20.0);
  rect.setYmin(5.0);
  rect.setYmax(15.0);
  PlotView view2(rect);

  for (PlotView *view : {&view1, &view2}) {
    view->setXZero(false);
    view->setYZero(false);
    view->setGrid(false);
  }


  
  window.add(&view1,"Fit");
  window.add(&view2,"Fit Accuracy");
  
  view1.add(&pf);

  PlotFunction1D pF=F;
  view1.add(&pF);
  PlotKey key1(-3, 120);
  key1.add(&pf, "data");
  key1.add(&pF, "fit");
  view1.add(&key1);
  
  {
    PlotStream xLabelStream(view1.xLabelTextEdit());
    xLabelStream << PlotStream::Clear()
		 << PlotStream::Center()
		 << PlotStream::Family("Sans Serif")
		 << PlotStream::Size(16)
		 << "t"
		 << PlotStream::EndP();
    
    PlotStream yLabelStream(view1.yLabelTextEdit());
    yLabelStream << PlotStream::Clear()
		 << PlotStream::Center()
		 << PlotStream::Family("Sans Serif")
		 << PlotStream::Size(16)
		 << "v(t)"
		 << PlotStream::EndP();
  }
  

  Eigen::MatrixXd errorMatrix=minimizer.getErrorMatrix();
  PlotErrorEllipse ellipse(pv0.getValue(),pg.getValue(),
			   errorMatrix(0,0),
			   errorMatrix(1,1),
			   errorMatrix(0,1));
  view2.add(&ellipse);

  PlotPoint point(15,9.8);
  {
    PlotPoint::Properties prop;
    prop.symbolSize=10;
    prop.brush.setStyle(Qt::SolidPattern);
    //prop.brush.setColor("darkBlue");
    point.setProperties(prop);
  }
  view2.add(&point);

  PlotKey key2(2,13);
  key2.add(&point, "input value");
  key2.add(&ellipse, "measurement");
  view2.add(&key2);

  QFont font;
  font.setPointSize(16);
  for (PlotKey * key : {&key1, &key2}) key->setFont(font);
  
  {
    PlotStream xLabelStream(view2.xLabelTextEdit());
    xLabelStream << PlotStream::Clear()
		 << PlotStream::Center()
		 << PlotStream::Family("Sans Serif")
		 << PlotStream::Size(16)
		 << "v"
		 << PlotStream::Sub()
		 << "0"
		 << PlotStream::Normal()
		 << PlotStream::EndP();
    
    
    PlotStream yLabelStream(view2.yLabelTextEdit());
    yLabelStream << PlotStream::Clear()
		 << PlotStream::Center()
		 << PlotStream::Family("Sans Serif")
		 << PlotStream::Size(16)
		 << "g"
		 << PlotStream::EndP();
  }


  window.show();
  app.exec();


  
  return 1;
}


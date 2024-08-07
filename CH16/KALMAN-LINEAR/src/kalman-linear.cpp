#include "QatPlotting/PlotStream.h"
#include "QatPlotWidgets/PlotView.h"
#include "QatPlotting/PlotProfile.h"
#include "QatPlotting/PlotFunction1D.h"
#include "QatPlotting/RealArg.h"
#include "QatGenericFunctions/Variable.h"
#include "QatGenericFunctions/Parameter.h"
#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include "Eigen/Dense"
#include <cmath>
#include <random>
using namespace std;

int main(int argc, char ** argv ) {

  QApplication     app(argc,argv);
  
  QMainWindow window;
  QToolBar *toolBar=window.addToolBar("Tools");
  QAction  *nextAction=toolBar->addAction("Next");
  
  nextAction->setShortcut(QKeySequence("n"));
  
  QObject::connect(nextAction, SIGNAL(triggered()), &app, SLOT(quit()));
  


  PRectF   rect;
  rect.setXmin(-5);
  rect.setXmax(5);
  rect.setYmin   (15);
  rect.setYmax(+30);
  PlotView view(rect);
  window.setCentralWidget(&view);

  const unsigned int seed=999;
  mt19937 engine;
  normal_distribution<double> gauss(seed);

  // MAKE A PROFILE PLOT
  PlotProfile profilePlot, profilePlot2;

  // MAKE IT BLUE
  {
    PlotProfile::Properties prop;
    prop.pen.setWidth(2);
    prop.pen.setColor(QColor("darkBlue"));
    profilePlot.setProperties(prop);
  }
  // MAKE IT Red
  {
    PlotProfile::Properties prop;
    prop.pen.setWidth(4);
    prop.pen.setColor(QColor("darkRed"));
    profilePlot2.setProperties(prop);
  }


  // SET THE PARAMETERS OF A FIT:
  Genfun::Parameter A0      ("A0",     1,   -20.0, 20.0);
  Genfun::Parameter A1      ("A1",     1,   -20.0, 20.0);

  Genfun::Variable X;
  Genfun::GENFUNCTION F=A0 + A1*X;

  double error = 0.02;
  std::vector<QPointF> Points;
  {
    // CARRY OUT THE FIRST PSEUDO EXPERIMENT
    for (double t=-5.0; t<5; t += 0.2) {
      double v = F(t);
      double dv = error*gauss(engine);
      QPointF point(t,v+dv);
      Points.push_back(point);
      profilePlot.addPoint(point,error);

    }
  }
  
  A0.setValue(2.0);
  A1.setValue(-1.0);
  
  PlotFunction1D v(F, RealArg::Gt(-5.0) && RealArg::Lt(5.0));
  {
    PlotFunction1D::Properties prop;
    prop.pen.setWidth(3);
    prop.pen.setColor(QColor("darkRed"));
    v.setProperties(prop);
  }
  
  // PLOT THE EXPERIMENT:
  {
    view.add(&profilePlot);
    view.add(&profilePlot2);
    view.add(&v);
    
    PlotStream (view.titleTextEdit())     <<PlotStream::Clear() << PlotStream::Center() << "Linear" << PlotStream::EndP();
    PlotStream (view.xLabelTextEdit())    <<PlotStream::Clear() << PlotStream::Center() << "x"       << PlotStream::EndP();
    
    view.show();
    window.show();
    view.setBox(false);
    app.exec();
    view.clear();
  }


  // Progressive Fit....
  for (int N=0;N<1;N++) {
    // Start the progressive fit:
    Eigen::MatrixXd CC(2,2);
    Eigen::VectorXd  A(2);
    A(0)=A0.getValue();
    A(1)=A1.getValue();
    CC(0,0)=10;
    CC(1,1)=10;
    CC(0,1)=CC(1,0)= 0;
    //    
    for (unsigned int i=0;i<Points.size();i++) {


      profilePlot2.addPoint(QPointF(Points[i].x(),Points[i].y()),error);;
      
      // BUILD THE MATRIX H AND ITS TRANSPOSE:

      Eigen::MatrixXd  H(1,2);
      H(0,0) =1;
      H(0,1) =Points[i].x();
      Eigen::MatrixXd HT=H.transpose();

      Eigen::MatrixXd R(1,1);
      R(0,0)=error*error;
      
 
      Eigen::MatrixXd CCP=(CC.inverse() + HT*R.inverse()*H).inverse(); 
      Eigen::VectorXd AP =   CCP*(CC.inverse()*A + HT*R.inverse()*((H*A)[0]+ Points[i].y()-F(Points[i].x())));

      CC=CCP;
      A =AP;

      A0.setValue(A[0]);
      A1.setValue(A[1]);

      view.add(&profilePlot);
      view.add(&profilePlot2);
      view.add(&v);
      view.show();
      window.show();
      app.exec();
      view.clear();

    }
  }


  return 0;

}

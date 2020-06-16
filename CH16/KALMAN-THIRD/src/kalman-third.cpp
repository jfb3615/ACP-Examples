#include "QatPlotting/PlotStream.h"
#include "QatPlotWidgets/PlotView.h"
#include "QatPlotting/PlotProfile.h"
#include "QatPlotting/PlotFunction1D.h"
#include "QatPlotting/PlotKey.h"
#include "QatPlotting/RealArg.h"
#include "QatGenericFunctions/Variable.h"
#include "QatGenericFunctions/Parameter.h"
#include <QtGui>
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
  rect.setXmin(-4);
  rect.setXmax(4);
  rect.setYmin   (-45);
  rect.setYmax(45);
  PlotView view(rect);
  view.setGrid(false);
  view.setXZero(false);
  view.setYZero(false);

  window.setCentralWidget(&view);

  mt19937 engine;
  normal_distribution<double> gauss;

  // MAKE A PROFILE PLOT
  PlotProfile profilePlot, profilePlot2;

  // MAKE IT UNFILLED
  {
    PlotProfile::Properties prop;
    prop.symbolSize=5;
    prop.brush.setStyle(Qt::SolidPattern);
    prop.pen.setColor(QColor("darkGray"));
    prop.brush.setColor(QColor("darkGray"));
    profilePlot.setProperties(prop);
  }
  // MAKE IT Filled
  {
    PlotProfile::Properties prop;
    prop.symbolSize=5;
    prop.brush.setStyle(Qt::SolidPattern);
    profilePlot2.setProperties(prop);
  }


  // SET THE PARAMETERS OF A FIT:
  Genfun::Parameter A0      ("A0",     1,   -20.0, 20.0);
  Genfun::Parameter A1      ("A1",     -1,   -20.0, 20.0);
  Genfun::Parameter A2      ("A2",     1,   -20.0, 20.0);
  Genfun::Parameter A3      ("A3",     -1,   -20.0, 20.0);

  Genfun::Variable X;
  Genfun::GENFUNCTION F=A0 + A1*X + A2*X*X + A3*X*X*X;

  double error = 2.0;
  std::vector<QPointF> Points;
  {
    // CARRY OUT THE FIRST PSEUDO EXPERIMENT
    for (double t=-3.0; t<3; t += 0.3) {
      double v = F(t);
      double dv = error*gauss(engine);
      QPointF point(t,v+dv);
      Points.push_back(point);
      profilePlot.addPoint(point,error);

    }
  }
  
  A0.setValue(-2.0);
  A1.setValue(2.0);
  A2.setValue(-2.0);
  A3.setValue(2.0);
  
  // Progressive Fit....
  for (int N=0;N<1;N++) {
    // Start the progressive fit:
    Eigen::MatrixXd CC=Eigen::MatrixXd::Zero(4,4);
    Eigen::VectorXd    A(4);
    A[0]=A0.getValue();
    A[1]=A1.getValue();
    A[2]=A2.getValue();
    A[3]=A3.getValue();
    CC(0,0)=10;
    CC(1,1)=10;
    CC(2,2)=10;
    CC(3,3)=10;
    PlotFunction1D v(F, RealArg::Gt(-5.0) && RealArg::Lt(5.0));
    {
      PlotFunction1D::Properties prop;
      prop.pen.setWidth(3);
      v.setProperties(prop);
    }
    
    // PLOT THE EXPERIMENT:
    {
      view.add(&profilePlot);
      view.add(&profilePlot2);
      view.add(&v);

      PlotKey key(-1.2, 30);
      key.add (&profilePlot, "Unused points");
      key.add (&profilePlot2, "Used points");
      key.add (&v,           "Fit");
      view.add(&key);
      
      PlotStream (view.titleTextEdit())     <<PlotStream::Clear() << PlotStream::Center() << PlotStream::Size(16) << "Cubic: step " << 0 << PlotStream::EndP();
      PlotStream (view.xLabelTextEdit())    <<PlotStream::Clear() << PlotStream::Center() << PlotStream::Size(16) << "x"       << PlotStream::EndP();
      PlotStream (view.yLabelTextEdit())    <<PlotStream::Clear() << PlotStream::Center() << PlotStream::Size(16) << "y"       << PlotStream::EndP();

      
      view.show();
      window.show();
      view.setBox(false);
      app.exec();
      view.clear();
    }

    //    std::random_shuffle(Points.begin(),Points.end());
    for (unsigned int i=0;i<Points.size();i++) {

     
      PlotStream (view.titleTextEdit())     <<PlotStream::Clear() << PlotStream::Center() << PlotStream::Size(16) << "Cubic: step " << int(i)+1 << PlotStream::EndP();
      PlotStream (view.xLabelTextEdit())    <<PlotStream::Clear() << PlotStream::Center() << PlotStream::Size(16) << "x"       << PlotStream::EndP();
      PlotStream (view.yLabelTextEdit())    <<PlotStream::Clear() << PlotStream::Center() << PlotStream::Size(16) << "y"       << PlotStream::EndP();
      

      profilePlot2.addPoint(QPointF(Points[i].x(),Points[i].y()),error);;
      
      // BUILD THE MATRIX H AND ITS TRANSPOSE:

      Eigen::MatrixXd H = Eigen::MatrixXd::Zero(1,4);
      H(0,0) =1;
      H(0,1) =Points[i].x();
      H(0,2) =Points[i].x()*Points[i].x();
      H(0,3) =Points[i].x()*Points[i].x()*Points[i].x();
      Eigen::MatrixXd HT=H.transpose();

      Eigen::MatrixXd R(1,1);
      R(0,0)=error*error;

      Eigen::MatrixXd CCP=   (CC.inverse() + HT*R.inverse()*H).inverse(); 
      Eigen::VectorXd AP =   CCP*(CC.inverse()*A + HT*R.inverse()*((H*A)[0]+ Points[i].y()-F(Points[i].x())));

      CC=CCP;
      A =AP;

      A0.setValue(A[0]);
      A1.setValue(A[1]);
      A2.setValue(A[2]);
      A3.setValue(A[3]);


      view.add(&v);
      view.add(&profilePlot);
      view.add(&profilePlot2);

      PlotKey key(-1.2, 30);
      key.add (&profilePlot, "Unused points");
      key.add (&profilePlot2, "Used points");
      key.add (&v,           "Fit");
      view.add(&key);
      
      app.exec();
      view.clear();

    }
  }


  return 0;

}

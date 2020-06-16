#include "QatPlotting/PlotStream.h"
#include "QatPlotWidgets/PlotView.h"
#include "QatPlotting/PlotFunction1D.h"
#include "QatPlotting/PlotProfile.h"
#include "QatPlotting/RealArg.h"
#include "QatGenericFunctions/Exp.h"
#include "QatGenericFunctions/Variable.h"
#include "QatGenericFunctions/Parameter.h"
#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include "Eigen/Dense"
#include <random>
#include <cmath>

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
  rect.setYmin   (-1.2);
  rect.setYmax(+1.2);
  PlotView view(rect);
  window.setCentralWidget(&view);

  random_device dev;
  mt19937 engine(dev());
  normal_distribution<double> gauss;
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
  Genfun::Parameter W      ("W",     .15,   0.1, 10.0);
  Genfun::Parameter THETA  ("THETA", 1,   -5,    5);

  Genfun::Variable X;
  Genfun::Exp      Exp;
  Genfun::GENFUNCTION S   = W*X+THETA;
  Genfun::GENFUNCTION F=(Exp(S) - Exp(-S))/ (Exp(S) + Exp(-S));




  double error = 0.05;
  std::vector<QPointF> Points;

  // GENERATE 20 CATS AND 20 DOGS;

  for (int t=-1;t<2;t+=2) {
    for (int j=0;j<20;j++) {
      double x=t+gauss(engine);
      QPointF point(x,t);
      Points.push_back(point);
      profilePlot.addPoint(point,0);
    }
  }
    




  
 
  // Progressive Fit....
  for (int N=0;N<1;N++) {
    // Start the progressive fit:
    Eigen::MatrixXd CC(2,2);
    Eigen::VectorXd    A(2);
    A[0]=W.getValue();
    A[1]=THETA.getValue();
    CC(0,0)=0.2;
    CC(1,1)=0.5;
    CC(0,1)= CC(1,0)=0;
    
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
      
      PlotStream (view.titleTextEdit())     <<PlotStream::Clear() << PlotStream::Center() << "Sigmoid" << PlotStream::EndP();
      PlotStream (view.xLabelTextEdit())    <<PlotStream::Clear() << PlotStream::Center() << "x"       << PlotStream::EndP();
      
      view.show();
      window.show();
      view.setBox(false);
      app.exec();
      view.clear();
    }
    
    std::random_shuffle(Points.begin(),Points.end());
    for (unsigned int i=0;i<Points.size();i++) {
      
      profilePlot2.addPoint(QPointF(Points[i].x(),Points[i].y()),error);;
      
      // BUILD THE MATRIX H AND ITS TRANSPOSE:
      
      double sechS  =1.0/cosh(A[0]*Points[i].x() + A[1]);
      double sech2S = sechS*sechS;
      Eigen::MatrixXd  H(1,2);
      H(0,0) =sech2S*Points[i].x();
      H(0,1) =sech2S*1;
      Eigen::MatrixXd HT=H.transpose();
      
      Eigen::MatrixXd R(1,1);
      R(0,0)=error*error;
      
      Eigen::MatrixXd CCP=(CC.inverse() + HT*R.inverse()*H).inverse(); 
      Eigen::VectorXd AP =   CCP*(CC.inverse()*A + HT*R.inverse()*((H*A)[0]+ Points[i].y()-F(Points[i].x())));
      
      CC=CCP;
      A =AP;

      W.setValue(A[0]);
      THETA.setValue(A[1]);

      view.add(&profilePlot);
      view.add(&profilePlot2);
      view.add(&v);
      app.exec();
      view.clear();
    }
  }


  return 0;

}

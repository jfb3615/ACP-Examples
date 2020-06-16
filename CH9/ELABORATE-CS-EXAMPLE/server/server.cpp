#include "QatDataAnalysis/ServerSocket.h"
#include "QatPlotting/PlotStream.h"
#include "QatPlotWidgets/PlotView.h"
#include "QatDataAnalysis/Hist1D.h"
#include "QatPlotting/PlotHist1D.h"
#include "QatPlotting/PlotProfile.h"
#include "QatPlotting/PlotFunction1D.h"
#include "QatPlotting/PlotErrorEllipse.h"
#include "QatPlotting/RealArg.h"
#include "QatDataModeling/MinuitMinimizer.h"
#include "QatGenericFunctions/NormalDistribution.h"
#include "QatGenericFunctions/Exp.h"
#include "QatGenericFunctions/AbsFunctional.h"
#include "QatGenericFunctions/Variable.h"
#include "Eigen/Dense"
#include <cmath>
#include <QMainWindow>
#include <QTextEdit>
#include <QApplication>
#include <QTimer>

int main(int argc, char ** argv ) {

  QApplication     app(argc,argv);

  try {
    // Create the socket
    unsigned int port=60000;
    ServerSocket *server = new ServerSocket ( port );
    
    while ( true ) {
      
      
      
      QMainWindow window;

      
      QTimer   *timer=new QTimer();
      timer->setInterval(50);
      timer->setSingleShot(false);
      QObject::connect(timer, SIGNAL(timeout()), &app, SLOT(quit()));
      
      
      PRectF   rect;
      rect.setXmin(-3);
      rect.setXmax(3);
      rect.setYmin  (-40);
      rect.setYmax(40);
      PlotView view(rect);
      window.setCentralWidget(&view);
      
      
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
      Genfun::Parameter A0      ("A0",     -1,   -20.0, 20.0);
      Genfun::Parameter A1      ("A1",     -2,   -20.0, 20.0);
      Genfun::Parameter A2      ("A2",     -2,   -20.0, 20.0);
      Genfun::Parameter A3      ("A3",     2,   -20.0, 20.0);
      
      
      Genfun::Variable X;
      Genfun::GENFUNCTION F=A0 + A1*X + A2*X*X + A3*X*X*X;
      
      double error = 1.0;
      
      // Start the progressive fit:
      using namespace Eigen;
      MatrixXd CC=MatrixXd::Zero(4,4);
      VectorXd A =VectorXd::Zero(4);
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
	prop.pen.setColor(QColor("darkRed"));
	v.setProperties(prop);
      }
      
  // PLOT THE EXPERIMENT:
      {
	view.add(&profilePlot);
	view.add(&profilePlot2);
	view.add(&v);
	
	PlotStream (view.titleTextEdit())     <<PlotStream::Clear() << PlotStream::Center() << "Cubic" << PlotStream::EndP();
	PlotStream (view.xLabelTextEdit())    <<PlotStream::Clear() << PlotStream::Center() << "x"       << PlotStream::EndP();
	
	view.show();
	window.show();
	view.setBox(false);
	timer->start();
	app.exec();
	view.clear();
      }
      
      
      std::cout << "Ready, listening on port  "  << port << std::endl;
      ServerSocket new_sock;
      server->accept ( new_sock );
      
      try {
	while ( true ) {
	  
	  int command;  
	  new_sock >> command;
	  
	  if (command==0) {
	    double x, y, dy;
	    new_sock >> x >> y >> dy;
	    profilePlot2.addPoint(QPointF(x,y),dy);;
	    
	    // BUILD THE MATRIX H AND ITS TRANSPOSE:
	    
	    MatrixXd  H=MatrixXd::Zero(1,4);
	    H(0,0) =1;
	    H(0,1) =x;
	    H(0,2) =x*x;
	    H(0,3) =x*x*x;
	    MatrixXd HT=H.transpose();
	    
	    MatrixXd R=MatrixXd::Zero(1,1);
	    R(0,0)=error*error;
	    
	    
	    MatrixXd CCP=MatrixXd::Zero(4,4); 
	    VectorXd AP;
	    
	    CCP=((CC.inverse() + HT*R.inverse()*H).inverse()); // WM
	    AP =CCP*(CC.inverse()*A + HT*R.inverse()*((H*A)[0]+ y-F(x)));
	    
	    CC=CCP;
	    A =AP;
	    
	    A0.setValue(A[0]);
	    A1.setValue(A[1]);
	    A2.setValue(A[2]);
	    A3.setValue(A[3]);
	    
	    
	    view.add(&profilePlot);
	    view.add(&profilePlot2);
	    view.add(&v);
	    
	    app.exec();
	    view.clear();
	  }
	  else  {
	    view.clear();
	    break;
	  }
	}
      }
      catch ( std::exception & e) {
	std::cout << "Exception was caught:" << e.what() << "Continuing" << std::endl;
      }
    }
  }
  catch ( std::exception& e ) {
    std::cout << "Exception was caught:" << e.what() << "\nExiting.\n";
  }
  return 0;
}

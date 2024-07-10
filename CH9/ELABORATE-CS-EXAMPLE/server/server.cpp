//
// This example has been heavily modified after the transition to QT
// version 6, which broke the example.
//

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
#include <QThread>
#include <unistd.h>


PlotProfile *profilePlot;
PlotView *view{nullptr};


// SET THE PARAMETERS OF A FIT:
Genfun::Parameter A0      ("A0",     -1,   -20.0, 20.0);
Genfun::Parameter A1      ("A1",     -2,   -20.0, 20.0);
Genfun::Parameter A2      ("A2",     -2,   -20.0, 20.0);
Genfun::Parameter A3      ("A3",     2,   -20.0, 20.0);


Genfun::Variable X;
Genfun::GENFUNCTION F=A0 + A1*X + A2*X*X + A3*X*X*X;

PlotFunction1D v(F, RealArg::Gt(-5.0) && RealArg::Lt(5.0));

// Create the socket
unsigned int port=60000;
ServerSocket *server = new ServerSocket ( port );

class Updater {

public:

  // This routine adds point to the plot and updates the parameters
  // A[0-3] of the fit.
  //
  void run()  {
    try {
   
      // MAKE IT BLUE
      {
	PlotProfile::Properties prop;
	prop.pen.setWidth(2);
	prop.pen.setColor(QColor("darkBlue"));
	profilePlot->setProperties(prop);
      }
      
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

      {
	PlotFunction1D::Properties prop;
	prop.pen.setWidth(3);
	prop.pen.setColor(QColor("darkRed"));
	v.setProperties(prop);
      }

      
      ServerSocket new_sock;
      server->accept ( new_sock );
      	    
      try {
	while ( true ) {
	  
	  int command;  
	  new_sock >> command;
	  
	  if (command==0) {
	    double x, y, dy;
	    new_sock >> x >> y >> dy;
	    profilePlot->addPoint(QPointF(x,y),dy);;
	    


	    double error = 1.0;
	    
	  
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
	    
	    
	  }
	  else  {
	    return;
	  }
	}
      }
      catch ( std::exception & e) {
	std::cout << "Exception was caught:" << e.what() << "Continuing" << std::endl;
      }

    }
    catch ( std::exception& e ) {
      std::cout << "Exception was caught:" << e.what() << "\nExiting.\n";
    }
  }
};


class SignalCatcher: public QObject{
  Q_OBJECT
public:

  PlotView *pV;
};



int main(int argc, char ** argv ) {

  QApplication     app(argc,argv);

  QMainWindow window;
  PRectF   rect;
  rect.setXmin(-3);
  rect.setXmax(3);
  rect.setYmin  (-40);
  rect.setYmax(40);
  view=new PlotView(rect);
  window.setCentralWidget(view);
  PlotStream (view->titleTextEdit())     <<PlotStream::Clear() << PlotStream::Center() << "Cubic" << PlotStream::EndP();
  PlotStream (view->xLabelTextEdit())    <<PlotStream::Clear() << PlotStream::Center() << "x"       << PlotStream::EndP();

  view->setBox(false);


  if (server) {
    std::cout << "Ready, listening on port  "  << port << std::endl;
  }

  
  while (1) {

    std::cout << "Ready, waiting" << std::endl;
    profilePlot=new PlotProfile;
    view->add(profilePlot);
    view->add(&v);
    view->startAnimating(10);
  


    view->show();
    window.show();

    Updater updater;

    std::thread t1(&Updater::run,&updater);
    app.exec();
    t1.join();
    view->clear();

    delete profilePlot;
    
    A0.setValue(-1);
    A1.setValue(-2);
    A2.setValue(-2);
    A3.setValue( 2);
		
  }
}
      


  



  

//#include "server.moc"

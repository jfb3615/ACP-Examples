#include "QatPlotWidgets/PlotView.h"
#include "QatPlotting/PlotStream.h"
#include "QatPlotting/PlotHist1D.h"
#include "QatPlotting/PlotFunction1D.h"
#include "QatDataAnalysis/Hist1D.h"
#include "QatGenericFunctions/Power.h"
#include "QatGenericFunctions/Variable.h"
#include "QatGenericFunctions/Exp.h"
#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <cstdlib>
#include <iostream>
#include <string>
#include <random>

int main (int argc, char * * argv) {

  std::random_device       dev;
  std::mt19937             engine(dev());
  std::normal_distribution gauss;
  // Automatically generated:-------------------------:

  std::string usage= std::string("usage: ") + argv[0]; 
  if (argc!=1) {
    std::cout << usage << std::endl;
    exit(0);
  }

  constexpr unsigned int N{10000};
  constexpr double       E{3*100000};
  constexpr double       m{1.0};
  constexpr double       kT{2.0/3.0*E/N};

  QApplication     app(argc,argv);
  
  QMainWindow window;
  QToolBar *toolBar=window.addToolBar("Tools");
  QAction  *quitAction=toolBar->addAction("Quit");
  
  quitAction->setShortcut(QKeySequence("q"));
  
  QObject::connect(quitAction, SIGNAL(triggered()), &app, SLOT(quit()));
  

  PlotView view({0,20,0,400.0});
  window.setCentralWidget(&view);

  Hist1D mbVelocityHist{"Velocities",100, 0, 20.0};

  // array of velocities
  std::vector<double> velocity(3*N);

  // initialization sweep
  double totalEnergy{0.0};
  {
    for (unsigned int i=0;i<3*N;i++) {
      double v_i=gauss(engine);
      totalEnergy+=0.5*m*v_i*v_i;
      velocity[i]=v_i;
    }
  }

  // normalization sweep
  {
    double fFactor=sqrt(E/totalEnergy);
    for (unsigned int i=0;i<3*N;i++) {
      velocity[i]*=fFactor;
     }
  }

  // Plot mb distribution
  for (unsigned int i=0;i<3*N;i+=3) {
    unsigned int X=i,Y=X+1,Z=Y+1;
    mbVelocityHist.accumulate(sqrt(velocity[X]*velocity[X]+
				   velocity[Y]*velocity[Y]+
				   velocity[Z]*velocity[Z]));
  }

  
  using namespace Genfun;
  using namespace std::numbers;
  Power p3by2(1.5);
  Exp   exp;
  Variable V;
  GENFUNCTION MBDist=p3by2(m/2.0/pi/kT)*4*pi*V*V*exp(-m*V*V/2.0/kT)*mbVelocityHist.binWidth()*N;

  PlotFunction1D pMBDist{MBDist};
  
  PlotHist1D mbVelocityPlot{mbVelocityHist};
  view.add(&mbVelocityPlot);
  view.add(&pMBDist);
  
  
  PlotStream titleStream(view.titleTextEdit());
  titleStream << PlotStream::Clear()
	      << PlotStream::Center() 
	      << PlotStream::Family("Arial") 
	      << PlotStream::Size(16)
	      << "Maxwell-Boltzmann distribution"
	      << PlotStream::EndP();
  
  
  PlotStream xLabelStream(view.xLabelTextEdit());
  xLabelStream << PlotStream::Clear()
	       << PlotStream::Center()
	       << PlotStream::Family("Arial")
	       << PlotStream::Size(16)
	       << "v [units of (3kT)³⸍²]"
	       << PlotStream::EndP();
  
  PlotStream yLabelStream(view.yLabelTextEdit());
  yLabelStream << PlotStream::Clear()
	       << PlotStream::Center()
	       << PlotStream::Family("Arial")
	       << PlotStream::Size(16)
	       << "# molecules"
	       << PlotStream::EndP();
  
  
  
  window.show();
  app.exec();
  return 1;
}


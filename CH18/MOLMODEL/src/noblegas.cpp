#include <iostream>
#include "Eigen/Dense"
#include "QatPlotWidgets/MultipleViewWindow.h"
#include "QatPlotWidgets/PlotView.h"
#include "QatPlotting/PlotProfile.h"
#include "QatPlotting/PlotHist1D.h"
#include "QatPlotting/PlotStream.h"
#include "QatDataAnalysis/OptParse.h"
#include "QatDataAnalysis/Hist1D.h"
#include <QApplication>
#include <QMainWindow>
#include <QLayout>
#include <QAction>
#include <QToolBar>
//define constants and parameters


using namespace std;
using namespace Eigen;

#include "ArgonLennardJones.h"

AbsModel *model{nullptr};


int main(int argc, char **argv)
{

  static constexpr double L=1.0;
  PlotProfile totalEnergyProf,moleculeEnergyProf,workDoneProf, potentialEnergyProf;
  PlotProfile xyProf,yzProf,zxProf;
  
  double       E{200.0};
  unsigned int N{100};
  unsigned int NSTEPS{40000};
  double a{0.025};
  double ePs{1.0}; // Interaction strength ("Leonard-Jones");
  bool Lattice{false};
  NumericInput input;
  input.declare("E",   "Energy of system",    E);
  input.declare("N",   "Number of Particles", N);
  input.declare("a",   "radius of sphere",    a);
  input.declare("EPS", "interaction strength",ePs);
  input.declare("NSTEPS", "number of steps (default 40000)",NSTEPS);
  input.declare("lattice", "start on a lattice?", Lattice);
  try {
    input.optParse(argc,argv);
  }
  catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    std::cerr << input.usage() << std::endl;
    return 1;
  }
  
  E=input.getByName<double> ("E");
  N=input.getByName<unsigned int> ("N");
  NSTEPS=input.getByName<unsigned int> ("NSTEPS");
  a=input.getByName<double> ("a");
  ePs=input.getByName<double> ("EPS");
  Lattice=input.getByName<bool> ("lattice");
  
  model =new ArgonLennardJones(N,1.0,a, ePs, E, Lattice);

  
  //---------------------------------------------------------
  // Now that we have reboundCollection, start visualizing!
  QApplication app(argc, argv);
  
  MultipleViewWindow mainwin;
  mainwin.resize(600,600);
  
  QToolBar *toolBar=mainwin.addToolBar("Tools");
  QAction  *quitAction=toolBar->addAction("Quit");
  
  quitAction->setShortcut(QKeySequence("q"));
  
  QObject::connect(quitAction, SIGNAL(triggered()), &app, SLOT(quit()));
  
  std::cout << "INITIAL VALUES:" << std::endl;
  std::cout << "Potential energy" << model->getPotentialEnergy() << std::endl;
  std::cout << "Kinetic energy"   << model->getKineticEnergy() << std::endl;
  for (unsigned int k=0;k<NSTEPS;k++) {
    model->takeStep(0.000001);
    double t=model->getTime();
    if (!(k++ % 10)) {
      workDoneProf.addPoint(t,model->getWorkOnEnvironment());
      moleculeEnergyProf.addPoint(t,model->getKineticEnergy());
      potentialEnergyProf.addPoint(t,model->getPotentialEnergy());
      totalEnergyProf.addPoint(t,model->getTotalEnergy()+model->getWorkOnEnvironment());
    }
  }
  
  PlotView energyView({0.0,0.004,0.0, 1200.0});
  energyView.add(&workDoneProf);
  energyView.add(&moleculeEnergyProf);
  energyView.add(&potentialEnergyProf);
  energyView.add(&totalEnergyProf);
  energyView.setLabelXSizePercentage(40.0);
  mainwin.add(&energyView,"Energy");

  {
    PlotProfile::Properties prop;
    prop.pen.setStyle(Qt::NoPen);
    prop.brush.setStyle(Qt::SolidPattern);
    prop.brush.setColor("blue");
    workDoneProf.setProperties(prop);
    prop.brush.setColor("red");
    moleculeEnergyProf.setProperties(prop);
    prop.brush.setColor("green");
    potentialEnergyProf.setProperties(prop);
    prop.brush.setColor("black");
    totalEnergyProf.setProperties(prop);
  }

  PlotView pairCorrView({0.0,0.5,0.0, 1200.0});
  mainwin.add(&pairCorrView,"Pair Corr. Fcn.");

  Hist1D pairCorrHist("Pair Correlation", 100, 0.0, 0.5);

  PRectF rect{-0.5,0.5,-0.5,0.5};
  PlotView xyView{rect},yzView{rect},zxView{rect};

  struct Labeling {
    std::string title;
    std::string xAxis;
    std::string yAxis;
  };


  std::map<PlotView *, Labeling> labeling = {{&energyView,{"Noble Gas Simulation","time","Energy"}},
					     {&pairCorrView,{"Pair Correlation Function","r","g(r)"}},
					     {&xyView,{"XY view","x","y"}},
					     {&yzView,{"YZ view","y","z"}},
					     {&zxView,{"ZX view","z","x"}}};
  
  for (PlotView *view : {&xyView,&yzView,&zxView}) {
    mainwin.add(view, labeling[view].title);
    view->setFixedHeight(view->width());
  }
  for (unsigned int i=0;i<N;i++) {
    for (unsigned int j=i+1;j<N;j++) {
       const VectorXd & x1=model->getPosition(i);
        const VectorXd & x2=model->getPosition(j);
        Vector3d x=x1-x2;
        // Apply the minimum image condition:
        for (unsigned int d=0;d<3;d++) {
          while (x[d] > L/2.0)  x[d]-=L;
          while (x[d] < -L/2.0) x[d]+=L;
        }
        double   r=x.norm();
	pairCorrHist.accumulate(r,1/r/r);
    }
  }

  PlotHist1D pairCorrPlot=pairCorrHist;
  pairCorrView.setRect(pairCorrPlot.rectHint());
  pairCorrView.add(&pairCorrPlot);

  for (unsigned int i=0;i<model->getNumParticles();i++) {
    xyProf.addPoint(model->getPosition(i).x(),model->getPosition(i).y());
    yzProf.addPoint(model->getPosition(i).y(),model->getPosition(i).z());
    zxProf.addPoint(model->getPosition(i).z(),model->getPosition(i).x());
  }

  xyView.add(&xyProf);
  yzView.add(&yzProf);
  zxView.add(&zxProf);
  for (PlotView *view : {&energyView,&pairCorrView,&xyView,&yzView,&zxView}) {
    PlotStream titleStream(view->titleTextEdit());
    titleStream << PlotStream::Clear()
		<< PlotStream::Center() 
		<< PlotStream::Family("Arial") 
		<< PlotStream::Size(16)
		<< labeling[view].title
		<< PlotStream::EndP();
    
    
    PlotStream xLabelStream(view->xLabelTextEdit());
    xLabelStream << PlotStream::Clear()
		 << PlotStream::Center()
		 << PlotStream::Family("Arial")
		 << PlotStream::Size(16)
		 << labeling[view].xAxis
		 << PlotStream::EndP();
    
    PlotStream yLabelStream(view->yLabelTextEdit());
    yLabelStream << PlotStream::Clear()
		 << PlotStream::Center()
		 << PlotStream::Family("Arial")
		 << PlotStream::Size(16)
		 << labeling[view].yAxis
		 << PlotStream::EndP();
    if (view==&energyView) {
      PlotStream statBoxStream(view->statTextEdit());
      statBoxStream << PlotStream::Clear()
		    << PlotStream::Left()
		    << PlotStream::Family("Arial")
		    << PlotStream::Size(16)
		    << PlotStream::Color("blue")
		    << "･ external work\n" 
		    << PlotStream::Color("red")
		    << "･ kinetic energy\n"
		    << PlotStream::Color("green")
		    << "･ potential energy\n"
		    << PlotStream::Color("black")
		    << "･ total"
		    << PlotStream::EndP();
    }
  }
  mainwin.show();
  app.exec();
}
  



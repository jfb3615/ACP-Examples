#include "QatPlotWidgets/MultipleViewWindow.h"
#include "QatPlotWidgets/PlotView.h"
#include "QatPlotting/PlotProfile.h"
#include "QatPlotting/PlotStream.h"
#include "QatDataAnalysis/OptParse.h"
#include <QApplication>
#include <QMainWindow>
#include <QLayout>
#include <QAction>
#include <QLabel>
#include <QToolBar>
#include <QChronoTimer>
#include <QTimer>
#include <QThread>
#include "Autocorr.h"
#include <random>
#include <thread>
#include <iostream>
#include <memory>
#include "Eigen/Dense"
#include "ArgonLennardJones.h"
#include "HardSpheres.h"
#include "Simulator.h"
#include "DisplayCase.h"
using namespace Eigen;



int main(int argc, char **argv)
{
#ifndef __APPLE__
  setenv("QT_QPA_PLATFORM","xcb",0);
#endif  

  std::string program=argv[0];
  std::string usage =
    program +
    " system=value: \n     1=hardspheres\n     2=lennard-jones\n" +
    "T=value/def=0.9\n" + 
    "N=value/def=3\n" +
    "a=value/def=0.01\n" +
    "step=value/def=1.0\n" +
    "EPS=value/def=0.8\n" + 
    "duration=value/def=1000\n";

  std::string examples =
    "Examples\n" +
    program + " a=0.10 \n" +
    program + " a=0.10 N=4 stepsize=0.25 duration=5000\n";
    if (argc==2 && std::string(argv[1])=="-h") {
    std::cout << "\n" << usage << std::endl;
    std::cout << "\n";
    std::cout << examples << std::endl;
    return 0;
  }
  
  double T{0.9};
  double N{3};
  double a{0.01};
  double ePs{0.8}; // Interaction strength ("Leonard-Jones"), units if Tc~ 120K for Argon. 
  double system{1};
  double stepSize{1.0};
  unsigned int duration{1000};
  unsigned int burninTime{400};
  
  NumericInput input;
  input.declare("T",   "Temperature",         T);
  input.declare("N",   "Number of Particles", N);
  input.declare("a",   "radius of sphere",    a);
  input.declare("EPS", "interaction strength",ePs);
  input.declare("stepsize", "interaction strength",stepSize);
  input.declare("system", "1=hardspheres,2=lennardjones", system); 
  input.declare("burnin", "burnin time ", burninTime); 
  input.declare("duration", "duration for autocorr", duration); 
  try {
    input.optParse(argc,argv);
  }
  catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    std::cerr << input.usage() << std::endl;
    return 1;
  }
  AbsModel *model{nullptr};
  T=input.getByName<double> ("T");
  N=input.getByName<unsigned int> ("N");
  a=input.getByName<double> ("a");
  ePs=input.getByName<double> ("EPS");
  system=input.getByName<unsigned int> ("system");
  stepSize=input.getByName<double> ("stepsize");
  duration=input.getByName<unsigned int> ("duration");
  burninTime=input.getByName<unsigned int> ("burnin");
  if (system==1) {
    HardSpheres *hardSpheresModel=new HardSpheres(N*N*N,1.0,a,T);
    model=hardSpheresModel;
  }
  else if (system==2) {
    ArgonLennardJones *lennardJonesModel =new ArgonLennardJones(N*N*N,1.0,a, ePs, T);
    model=lennardJonesModel;
  }
  else {
    std::cerr << usage << std::endl;
    return 1;
  }

  static constexpr double L{1.0};
  double η=N*N*N*(4.0/3.0*std::numbers::pi)*std::pow(a,3);
  
  std::cout << "a                            : " <<  a << std::endl;  
  std::cout << "η                            : " <<  η << std::endl;  
  std::cout << "density                      : " << std::pow(N*a/L,3) << std::endl;
  std::cout << "In units of critical density : " << std::pow(N*a/L/0.682,3) << std::endl;
  std::cout << "In units of 3-point density  : " << std::pow(N*a/L/0.943,3) << std::endl;


  
  //---------------------------------------------------------
  // Now that we have reboundCollection, start visualizing!
  QApplication app(argc, argv);

  MultipleViewWindow mainwin;
  mainwin.resize(600,600);

  QToolBar *toolBar=mainwin.addToolBar("Tools");
  QAction  *quitAction=toolBar->addAction("Quit");
  quitAction->setShortcut(QKeySequence("q"));
  QObject::connect(quitAction,&QAction::triggered,&app,&QApplication::quit);

  size_t NPROCESSORS=std::thread::hardware_concurrency();
  size_t NTHREADS=NPROCESSORS-2;//std::max<unsigned int> (2U,NPROCESSORS)-1;
  std::vector<Simulator *> simulator;
  for (size_t i=0;i<NTHREADS;i++) simulator.emplace_back(new Simulator(model,stepSize,duration,burninTime));

  DisplayCase displayCase(duration);

  std::vector<QThread> thread{NTHREADS};
  for (size_t i=0;i<NTHREADS;i++) {
    simulator[i]->moveToThread(&thread[i]);
    QTimer *timer = new QTimer;
    timer->setSingleShot(true);
    QObject::connect(timer,&QTimer::timeout,simulator[i], &Simulator::simulate);
    thread[i].start();
    timer->start();
    displayCase.add(simulator[i]);
  }


  
  
  mainwin.add(displayCase.getAcceptanceRateView(),"AcceptanceRate");
  mainwin.add(displayCase.getAutocorrView(),"Autocorrelation");
 mainwin.add(displayCase.getPairCorrView(),"Pair Correlation");

 
  QTimer *twoTimer=new QTimer;
  twoTimer->setInterval(1000);
  QObject::connect(twoTimer,&QTimer::timeout,&displayCase, &DisplayCase::update);
  twoTimer->start();

  mainwin.show();
  app.exec();

  for (QThread & t: thread) {  
    t.requestInterruption();
    t.quit();
    t.wait();
  }
  for (Simulator *s : simulator) delete s;
}

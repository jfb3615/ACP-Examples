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
using namespace Eigen;





class Simulator: public QObject {

  Q_OBJECT

public:

  Simulator(AbsModel *model,double stepSize, unsigned int duration):model(model),stepSize(stepSize),duration(duration),
					     autocorrProf(std::make_unique<PlotProfile>()) {
    PlotProfile::Properties prop;
    prop.pen.setStyle(Qt::NoPen);
    prop.brush.setStyle(Qt::SolidPattern);
    prop.brush.setColor("blue");
    autocorrProf->setProperties(prop);
    acceptanceRateProf.setProperties(prop);
    
    for (int i=0;i<100;i++) {
      autocorrs.emplace_back(duration/100*(i+1));
      for (unsigned int p=0;p<model->getNumParticles();p++) {
	const double *x=&model->getPosition(p).x();
	const double *y=&model->getPosition(p).y();
	const double *z=&model->getPosition(p).z();
	autocorrs.back().followVariable(x);
	autocorrs.back().followVariable(y);
	autocorrs.back().followVariable(z);
      }
    }
  }

  PlotProfile &getAcceptanceRateProf()  { return acceptanceRateProf;}
  PlotProfile &getAutocorrProf()  const {return *autocorrProf;};
  double       getStepSize()      const {return stepSize;}
  
private:
  
  AbsModel *model{nullptr};
  double                       stepSize{1.0};
  unsigned int                 duration{1000};
  std::vector<Autocorr>        autocorrs;
  std::unique_ptr<PlotProfile> autocorrProf{nullptr};
  PlotProfile                  acceptanceRateProf{};
  
public slots:
  
  void simulate();


};

class DisplayCase: public QObject {
  Q_OBJECT

public:

  DisplayCase(unsigned int duration);
  PlotView *getAcceptanceRateView()  {return &acceptanceRateView;}
  PlotView *getAutocorrView()        {return &autocorrView;}

public slots:
  void update();

private:

  unsigned int duration{1000};
  PlotView acceptanceRateView{PRectF{0.0,1000,0.0, 1.2}};
  PlotView autocorrView;


  
};

void DisplayCase::update() {
  acceptanceRateView.recreate();
  autocorrView.recreate();
}

DisplayCase::DisplayCase(unsigned int duration):
  duration(duration),
  autocorrView({0.0,(double) duration,-1.2, 1.2}){
  
  std::map<PlotView *, std::string> yLabel={{&acceptanceRateView, "Acceptance Rate"}, {&autocorrView, "Autocorrelation"}};
  for (PlotView * v: {&acceptanceRateView,&autocorrView} ) {
    PlotStream xLabelStream(v->xLabelTextEdit());
    xLabelStream << PlotStream::Clear()
		 << PlotStream::Center()
		 << PlotStream::Family("Arial")
		 << PlotStream::Size(16)
		 << "step/1000"
		 << PlotStream::EndP();
    
    PlotStream yLabelStream(v->yLabelTextEdit());
    yLabelStream << PlotStream::Clear()
		 << PlotStream::Center()
		 << PlotStream::Family("Arial")
		 << PlotStream::Size(16)
		 << yLabel[v]
		 << PlotStream::EndP();
  }
}


void Simulator::simulate()
{
  std::cout << "HELLO from Simulator: starting simulation" << std::endl;
  //
  // Call time evolution to update position array x[]:
  //
  unsigned int k{0};
  unsigned int successes{0},totals{0};
  while (!thread()->isInterruptionRequested()) {
    bool accept=model->takeStep(stepSize);
    if (accept) successes++;
    totals++;
    
    for (Autocorr & a: autocorrs) a.addDataPoint(k);
    if (!(k++ % 1000)) {
      double acceptRate=double(successes)/double(totals);
      acceptanceRateProf.addPoint(k/1000,acceptRate);
      successes=totals=0;
      
    }
  }
  return;
    
    
     
    // pv_a->clear();
    // delete autocorrProf;
    // autocorrProf = new PlotProfile();
    
    // for (Autocorr & a: autocorrs) {
    //   a.compute();
  // }
    
      // for (Autocorr & a: autocorrs) {
    //   double result=a.harvest();
    //   if (std::isfinite(result)) autocorrProf->addPoint(a.getT(),result);
    // }
    // pv_a->add(autocorrProf);
    
  //   std::map<PlotView *, std::string> tLabel={{pv_a,  std::to_string(k)+" steps taken"},
  // 					      {pv_g,  "Acceptance Rate now="+std::to_string(acceptRate)}};
  //   if (0) for (PlotView *v : {pv_a, pv_g})  {
  //     PlotStream titleStream(v->titleTextEdit());
  //     titleStream << PlotStream::Clear()
  // 		  << PlotStream::Center() 
  // 		  << PlotStream::Family("Arial") 
  // 		  << PlotStream::Size(16)
  // 		  << tLabel[v] 
  // 		  << PlotStream::EndP();
  //   }
    
 
  //   pv_g->recreate(); // refresh the view. 
  //   pv_a->recreate();
  
  // }

}


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
  NumericInput input;
  input.declare("T",   "Temperature",         T);
  input.declare("N",   "Number of Particles", N);
  input.declare("a",   "radius of sphere",    a);
  input.declare("EPS", "interaction strength",ePs);
  input.declare("stepsize", "interaction strength",stepSize);
  input.declare("system", "1=hardspheres,2=lennardjones", system); 
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
  Simulator *simulator=new Simulator(model,stepSize,duration);
  DisplayCase displayCase(duration);
  
  displayCase.getAcceptanceRateView()->add(&simulator->getAcceptanceRateProf());
  mainwin.add(displayCase.getAcceptanceRateView(),"AcceptanceRate");
  mainwin.add(displayCase.getAutocorrView(),"Autocorrelation");

  size_t NPROCESSORS=std::thread::hardware_concurrency();
  QThread thread;
  simulator->moveToThread(&thread);
  QTimer *timer = new QTimer;
  timer->setSingleShot(true);
  QObject::connect(timer,&QTimer::timeout,simulator, &Simulator::simulate);
  thread.start();
  timer->start();

  QTimer *twoTimer=new QTimer;
  twoTimer->setInterval(1000);
  QObject::connect(twoTimer,&QTimer::timeout,&displayCase, &DisplayCase::update);
  twoTimer->start();

  mainwin.show();
  app.exec();

  
  thread.requestInterruption();
  thread.quit();
  thread.wait();
}
#include "runnoble.moc"

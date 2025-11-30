#include <Inventor/Qt/SoQt.h>
#include "QatPlotWidgets/MultipleViewWindow.h"
#include "QatPlotWidgets/PlotView.h"
#include "QatPlotting/PlotProfile.h"
#include "QatPlotting/PlotStream.h"
#include "QatDataAnalysis/OptParse.h"
#include <Inventor/Qt/viewers/SoQtExaminerViewer.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/sensors/SoTimerSensor.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <QApplication>
#include <QMainWindow>
#include <QLayout>
#include <QAction>
#include <QLabel>
#include <QToolBar>
#include <QDoubleSpinBox>
#include "Autocorr.h"
#include <random>
#include <thread>
#include <iostream>
#include "Eigen/Dense"
#include "ArgonLennardJones.h"
#include "HardSpheres.h"


using namespace Eigen;



// Some globals used in the callback function:
PlotProfile acceptanceRateProf;
PlotProfile *autocorrProf{nullptr};
PlotView *pv_g{nullptr};
PlotView *pv_a{nullptr};
std::vector<SoTranslation*> sphereTranslations;
double stepSize=1.0;

AbsModel *model{nullptr};
HardSpheres   *hardSpheresModel{nullptr};
ArgonLennardJones *lennardJonesModel{nullptr};
std::vector<Autocorr> autocorrs;


//=======================================================
//
// The signal catcher is here in case one would like
// to interactively adjust the stepsize.  It is connected
// to the spin box in the title bar.
//
class SignalCatcher:public QObject {

  Q_OBJECT

public:

  SignalCatcher(double & stepSize):stepSize(stepSize){}
  
public slots:

 
  void setValue(double s) {stepSize=s;}

private:

  double &stepSize;
};


//=======================================================
//
//  timeSensorCallback
//  1. proceeds to the next MCMC step.
//  2. records information about acceptance rate.
//  3. records information about autocorrelation
//  4. Every thousandth step:
//      a) calculates the acceptance rate, and add a 
//         point to the acceptance rate profile
//      b) calculates the autocorrelation, deletes
//         recreates and fills the autocorrelation profile
//      c) labels the plots and sets plot properties. 
//      d) refreshes the plots so they are redrawn.
//
//=======================================================
void timeSensorCallback(void * , SoSensor * )
{
  //
  // Call time evolution to update position array x[]:
  //
  static unsigned int k{0};
  static unsigned int successes{0},totals{0};
  bool accept=model->takeStep(stepSize);
  if (accept) successes++;
  totals++;
  for (Autocorr & a: autocorrs) a.addDataPoint(k);

  
  if (!(k++ % 1000)) {
    
    
    double acceptRate=double(successes)/double(totals);
    acceptanceRateProf.addPoint(k/1000,acceptRate);
    successes=totals=0;
    
    pv_a->clear();
    delete autocorrProf;
    autocorrProf = new PlotProfile();
    
    std::vector<std::thread> tVector;
    for (Autocorr & a: autocorrs) {
      tVector.push_back(std::thread(&Autocorr::compute, &a));
    }
    
    for (std::thread & t: tVector) t.join();
    
    for (Autocorr & a: autocorrs) {
      double result=a.harvest();
      if (std::isfinite(result)) autocorrProf->addPoint(a.getT(),result);
    }
    pv_a->add(autocorrProf);
    
    std::map<PlotView *, std::string> tLabel={{pv_a,  std::to_string(k)+" steps taken"},
					      {pv_g,  "Acceptance Rate now="+std::to_string(acceptRate)}};
    for (PlotView *v : {pv_a, pv_g})  {
      PlotStream titleStream(v->titleTextEdit());
      titleStream << PlotStream::Clear()
		  << PlotStream::Center() 
		  << PlotStream::Family("Arial") 
		  << PlotStream::Size(16)
		  << tLabel[v] 
		  << PlotStream::EndP();
    }
    
    {
      PlotProfile::Properties prop;
      prop.pen.setStyle(Qt::NoPen);
      prop.brush.setStyle(Qt::SolidPattern);
      prop.brush.setColor("blue");
      autocorrProf->setProperties(prop);
      acceptanceRateProf.setProperties(prop);
    }

    pv_g->recreate(); // refresh the view. 
    pv_a->recreate();
  
  }

  //
  // Set the position of all the points in the point set,
  // which represent molecules:
  //
  for(unsigned int i=0;i<model->getNumParticles();i++) 
    {
      const Vector3d &x=model->getPosition(i);
      if (hardSpheresModel || lennardJonesModel) sphereTranslations[i]->translation.setValue(x.y(),x.z(), x.x());
    }
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
  double system=1;
  unsigned int duration=1000;
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

  T=input.getByName<double> ("T");
  N=input.getByName<unsigned int> ("N");
  a=input.getByName<double> ("a");
  ePs=input.getByName<double> ("EPS");
  system=input.getByName<unsigned int> ("system");
  stepSize=input.getByName<double> ("stepsize");
  duration=input.getByName<unsigned int> ("duration");
  if (system==1) {
    hardSpheresModel=new HardSpheres(N*N*N,1.0,a,T);
    model=hardSpheresModel;
  }
  else if (system==2) {
    lennardJonesModel =new ArgonLennardJones(N*N*N,1.0,a, ePs, T);
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
  SoQt::init(&mainwin);

  QToolBar *toolBar=mainwin.addToolBar("Tools");
  QAction  *quitAction=toolBar->addAction("Quit");

  QLabel *stepSizeLabel=new QLabel;
  stepSizeLabel->setText(" Step size:");
  
  QDoubleSpinBox *stepSizeSpinBox=new QDoubleSpinBox();
  stepSizeSpinBox->setRange(0.0,1.0);
  stepSizeSpinBox->setValue(stepSize);
  stepSizeSpinBox->setSingleStep(0.0001);
  stepSizeSpinBox->setDecimals(4);
  toolBar->addWidget(stepSizeLabel);
  toolBar->addWidget(stepSizeSpinBox);
  
  quitAction->setShortcut(QKeySequence("q"));

  SignalCatcher signalCatcher(stepSize);
  
  
  QObject::connect(stepSizeSpinBox, &QDoubleSpinBox::valueChanged, &signalCatcher, &SignalCatcher::setValue);
  QObject::connect(quitAction, &QAction::triggered, &app, &QApplication::quit);
  



  
  
  SoSeparator *root = new SoSeparator;
  root->ref();
  
  // Schedule the update right here: 
  SoTimerSensor * timeSensor = new SoTimerSensor;
  timeSensor->setFunction(timeSensorCallback);
  timeSensor->setBaseTime(SbTime::getTimeOfDay());
  timeSensor->setInterval(0.0001f);
  timeSensor->schedule();

  // Set draw style and light model for the molecules:
  // "Lines", drawn with "Base Color"
  SoDrawStyle *molDrawStyle=new SoDrawStyle();
  molDrawStyle->style=SoDrawStyle::LINES;
  molDrawStyle->pointSize=3;
  root->addChild(molDrawStyle);

  SoLightModel *molLightModel = new SoLightModel;
  molLightModel->model=SoLightModel::BASE_COLOR;
  root->addChild(molLightModel);

  SoBaseColor *baseRed=new SoBaseColor;
  baseRed->rgb.setValue(1.0,0.0,0.0);
  root->addChild(baseRed);

  SoBaseColor *baseGreen=new SoBaseColor;
  baseGreen->rgb.setValue(0.0,1.0,0.0);


  

  {
    SoSeparator *cubeSep=new SoSeparator;
    SoBaseColor *cubeColor=new SoBaseColor;
    cubeColor->rgb.setValue(0.2,0.2,1.0);
    SoCube *cube=new SoCube;
    cube->width.setValue(1.0);
    cube->height.setValue(1.0);
    cube->depth.setValue(1.0);
    cubeSep->addChild(cubeColor);
    cubeSep->addChild(cube);
    root->addChild(cubeSep);
    SoSphere *sphere = new SoSphere();
    if (hardSpheresModel) sphere->radius.setValue(hardSpheresModel->getRadius());
    if (lennardJonesModel) sphere->radius.setValue(lennardJonesModel->getRadius());

    std::random_device dev;
    std::mt19937 engine(dev());

    std::uniform_int_distribution<unsigned int> chooseGreenOne(0,N*N*N-1);
    unsigned int selectedOne=chooseGreenOne(engine);
    for (unsigned int i=0;i<N*N*N;i++) {
      SoSeparator *sep=new SoSeparator;
      if (i==selectedOne) sep->addChild(baseGreen);
      sphereTranslations.push_back(new SoTranslation);
      sep->addChild(sphereTranslations.back());
      sep->addChild(sphere);
      root->addChild(sep);
    }
  }

  
  // Use one of the convenient SoQt viewer classes.
  QWidget *evContainer=new QWidget;
  evContainer->setLayout(new QHBoxLayout);
  SoQtExaminerViewer * eviewer = new SoQtExaminerViewer();
  evContainer->layout()->addWidget(eviewer->getWidget());
  mainwin.add(evContainer,"Display");
  
  //  eviewer->setTransparencyType(SoGLRenderAction::SCREEN_DOOR);
  eviewer->setDoubleBuffer(false);
  eviewer->setSceneGraph(root);

  PlotView acceptanceRateView({0.0,1000,0.0, 1.2});
  acceptanceRateView.add(&acceptanceRateProf);
  mainwin.add(&acceptanceRateView,"AcceptanceRate");
  pv_g=&acceptanceRateView;  

  PlotView autocorrView({0.0,(double) duration,-1.2, 1.2});
  mainwin.add(&autocorrView,"Autocorrelation");
  pv_a=&autocorrView;  

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

  // Pop up the main window.
  SoQt::show(&mainwin);
  // Loop until exit.
  SoQt::mainLoop();

  // Clean up resources.
  delete eviewer;
  root->unref();



}
#include "runmcmc.moc"

#include <Inventor/Qt/SoQt.h>
#include <Inventor/Qt/viewers/SoQtExaminerViewer.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/sensors/SoTimerSensor.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoCamera.h>
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include "QatGenericFunctions/Variable.h"
#include "QatGenericFunctions/Cos.h"
#include "QatGenericFunctions/Sin.h"
#include "QatGenericFunctions/Power.h"
#include "QatGenericFunctions/RungeKuttaClassicalSolver.h"
#include "QatGenericFunctions/PhaseSpace.h"
#include "QatGenericFunctions/RootFinder.h"
#include "QatDataAnalysis/OptParse.h"
#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QToolBar>
#include <QAction>
#include <cstdlib>
#include <iostream>
#include <string>
#include <cmath>
#include "Eigen/Dense"
#include "unsupported/Eigen/MatrixFunctions"

double             timeUnit=1.0;
int                step=0;
SoTranslation     *trans=NULL;
SoPointSet        *pointSet=NULL;
SoCoordinate3     *coordinates=NULL;
bool               mapOn=false;
bool               threeDOn=false;
Eigen::Matrix2d    AMatrix;

using namespace Genfun;


// These three functions describe the coordinate vs time:
const AbsFunction *x=NULL, *y=NULL,*px=NULL, *py= NULL;


// Invoked at regular intervals to update the electron position:
static void timeSensorCallback(void * , SoSensor * )
{
  double t=step++/5.0;
  t/=timeUnit;


  // Go to eigenmodes:
  Eigen::Vector2d XV((*x)(t),(*y)(t));
  Eigen::Vector2d PV((*px)(t),(*py)(t));
  Eigen::Vector2d Xt=AMatrix*XV;
  Eigen::Vector2d Pt=AMatrix*PV;
  
  Eigen::Vector3d v1(Pt(0),0, Xt(0));
  Eigen::Vector3d yHat(0,1,0);
  Eigen::Vector3d V=v1+Xt(1)*v1 + Pt(1)*yHat;
  
  double X=V(0),Y=V(1), Z= (threeDOn ? V(2):0.0);
  static bool XPos = X>0;
  static int count=0;
  if (!mapOn) {
    if (!std::isfinite(X) || !std::isfinite(Y)) {
      step=0;
      count=0;
      pointSet->numPoints=count;
      return;
    }
    trans->translation.setValue(X,Y,Z); 
    coordinates->point.set1Value(count,X,Y,Z);
    pointSet->numPoints=count;
    count++;
  }
  else {
    trans->translation.setValue(X,Y,Z); 
    if ((X<=0&& XPos) || (X>0 && (!XPos))) {
      if (!std::isfinite(X) || !std::isfinite(Y)) {
	step=0;
	count=0;
	pointSet->numPoints=count;
	return;
      }
      NewtonRaphson NR(*x,0.001);
      try {
	double tPC=NR.root(t);
	double X=(*x)(tPC),Y=(*y)(tPC), Z=(*py)(tPC);
	trans->translation.setValue(X,Y,Z); 
	coordinates->point.set1Value(count,X,Y,Z);
	pointSet->numPoints=count;
	count++;
      }
      catch (std::runtime_error &e) {
	std::cout << e.what();
      }
    }
  }
  XPos=X>0;
}

// Main program:

int main (int argc, char * * argv) {

#ifndef __APPLE__
  setenv ("QT_QPA_PLATFORM","xcb",0);
#endif  
  
  const std::string usage=std::string(argv[0]) + " [THETA1=val] [THETA2=val] [PTHETA1=val] [PTHETA2=val] [TimeUnit=Val] [-m] [-3]";
  //
  // Parse the input ------------------------------------------------------
  //
  NumericInput input;
  input.declare("TimeUnit", "Unit of Time", 1.0);
  input.declare("THETA1",   "Initial value", 0.1);
  input.declare("THETA2",   "Initial value", 0.1);
  input.declare("PTHETA1",   "Initial value", 0.01);
  input.declare("PTHETA2",   "Initial value", 0.01);
  try {
    input.optParse(argc, argv);
  }
  catch (std::exception &) {
    std::cout << input.usage() << std::endl;
    std::cout << "Parse error; usage= " << usage << std::endl;
    exit(0);
  }
  bool fast=false;
  for (int i=1; i<argc;i++) {
    std::string thisArg(argv[i]);
    if (thisArg=="-m") {
      std::cout << "Map mode on" << std::endl;
      mapOn=true;
      std::copy(argv+i+1, argv+argc, argv+i);
      argc -= 1;
      i    -= 1;
    }
    else if (thisArg=="-3") {
      std::cout << "3D mode on" << std::endl;
      threeDOn=true;
      std::copy(argv+i+1, argv+argc, argv+i);
      argc -= 1;
      i    -= 1;
    }
    else if (thisArg=="-f") {
      std::cout << "fast mode on" << std::endl;
      fast=true;
      std::copy(argv+i+1, argv+argc, argv+i);
      argc -= 1;
      i    -= 1;
    }

  }

  if (argc!=1) {
    std::cout << "usage: " << usage << std::endl;
    exit(0);
  }


  timeUnit=input.getByName("TimeUnit");
  //-------------------------------------------------------------------------

  //
  // Initialize graphics:
  //
  QApplication     app(argc,argv);
  QMainWindow mainwin;
  mainwin.setMinimumWidth(800);
  mainwin.setMinimumHeight(800);
  SoQt::init(&mainwin);
  
  SoSeparator *root = new SoSeparator;
  root->ref();
  //
  // Schedule the update: 
  //
  SoTimerSensor * timeSensor = new SoTimerSensor;
  timeSensor->setFunction(timeSensorCallback);
  timeSensor->setBaseTime(SbTime::getTimeOfDay());
  timeSensor->setInterval(fast? 0.0002f: 0.02f);
  timeSensor->schedule();
  //
  // Make a blue nucleus:
  //
  {
    SoSeparator *sep = new SoSeparator();
    root->addChild(sep);
    
    QColor color("blue");
    SoMaterial *originMaterial = new SoMaterial;
    originMaterial->diffuseColor.setValue(color.red()/255.0, color.green()/255.0, color.blue()/255.0);
    sep->addChild(originMaterial);
    
    SoSphere *sphere = new SoSphere();
    sphere->radius.setValue(0.01);
    sep->addChild(sphere);
  }

  //
  // Make a:
  // 
  {
    SoSeparator *sep = new SoSeparator();
    root->addChild(sep);
    
    QColor color("red");
    SoMaterial *dotMaterial = new SoMaterial;
    dotMaterial->diffuseColor.setValue(color.red()/255.0, color.green()/255.0, color.blue()/255.0);
    sep->addChild(dotMaterial);
    
    trans = new SoTranslation();
    trans->translation.setValue(1,0,0);
    sep->addChild(trans);

    SoSphere *sphere = new SoSphere();
    sphere->radius.setValue(0.01);
    sep->addChild(sphere);

    SoSeparator *aSep=new SoSeparator();
    root->addChild(aSep);
    for (int i=0;i<3;i++) {
      SoCoordinate3 *arrow = new SoCoordinate3();
      arrow->point.set1Value(0,0,0,0);
      arrow->point.set1Value(1,i==0,i==1,i==2);
      aSep->addChild(arrow);
      
      SoDrawStyle *ds= new SoDrawStyle();
      ds->style=SoDrawStyle::LINES;
      ds->lineWidth=3;
      aSep->addChild(ds);
      
      SoMaterial *aMaterial = new SoMaterial();
      aMaterial->diffuseColor.setValue(i==0,i==1,i==2);
      aSep->addChild(aMaterial);
      
      SoLineSet *line = new SoLineSet();
      line->numVertices=2;
      aSep->addChild(line);
    }
  }


  {

    coordinates = new SoCoordinate3();
    root->addChild(coordinates);
    pointSet =new SoPointSet;
    root->addChild(pointSet);

  }
  //
  // Make an examiner viewer:
  //
  SoQtExaminerViewer * eviewer = new SoQtExaminerViewer();
  eviewer->setDoubleBuffer(false);
  //  
  // Make a tool bar with a quit button, and set the 
  // Keyboard accelerator:
  //
  QToolBar *toolBar=mainwin.addToolBar("Tools");
  QAction  *quitAction=toolBar->addAction("Quit");
  quitAction->setShortcut(QKeySequence("q"));
  QObject::connect(quitAction, SIGNAL(triggered()), &app, SLOT(quit()));
  
  
  //
  // Lay out in a box (this allows to add some graphs later if 
  // needed):
  //
  QVBoxLayout *layout=new QVBoxLayout();
  layout->addWidget(eviewer->getWidget());
  QWidget *widget=new QWidget();
  widget->setLayout(layout);
  mainwin.setCentralWidget(widget);
  eviewer->setSceneGraph(root);
  //
  // Position the camera (overriding some inconvenient defaults)
  //
  eviewer->getCamera()->position.setValue(0,0,6);
  eviewer->getCamera()->focalDistance=6;
  eviewer->saveHomePosition();


  // Here is the numerical part:---------------------------//
  double m=1,L=1,g=10;
  // ------------------------------------------------------
  //
  // 1. Phase space is 2x3D:
  //
  Classical::PhaseSpace phaseSpace(2);
  const Classical::PhaseSpace::Component 
    & r=phaseSpace.coordinates(),
    & p=phaseSpace.momenta();
  //
  // 2. Set the initial conditions:
  //
  //
  //
  double theta1=input.getByName("THETA1");
  double theta2=input.getByName("THETA2");
  double p1=input.getByName("PTHETA1");
  double p2=input.getByName("PTHETA2");

  phaseSpace.start(r[0], theta1);  // Fixed...
  phaseSpace.start(r[1], theta2);   // y is specified.
  phaseSpace.start(p[0], p1);    // px is determined
  phaseSpace.start(p[1], p2);    // py is specified.
  //
  // 3.  Henon-Heiles Hamiltonian 
  //
  GENFUNCTION T= (p[0]*p[0]+2*p[1]*p[1]-2.0*Cos()(r[0]-r[1])*p[0]*p[1])/(2.0*m*L*L)/(1+Power(2)(Sin()(r[0]-r[1])));
  GENFUNCTION V=-m*g*L*(2*Cos()(r[0])+Cos()(r[1]));
  GENFUNCTION H=T+V;

  Argument a={0,0,0,0};
  
  Eigen::Matrix2d TIJ;
  TIJ(0,0)=T.partial(2).partial(2)(a);   TIJ(0,1)=T.partial(2).partial(3)(a);
  TIJ(1,0)=T.partial(3).partial(2)(a);   TIJ(1,1)=T.partial(3).partial(3)(a);

  Eigen::Matrix2d VIJ;
  VIJ(0,0)=V.partial(0).partial(0)(a);   VIJ(0,1)=V.partial(0).partial(1)(a);
  VIJ(1,0)=V.partial(1).partial(0)(a);   VIJ(1,1)=V.partial(1).partial(1)(a);

  Eigen::Matrix2d O2=TIJ.sqrt().inverse()*VIJ*TIJ.sqrt().inverse();
  Eigen::SelfAdjointEigenSolver<Eigen::Matrix2d> esolver(O2);
  AMatrix=esolver.eigenvectors().transpose()*TIJ.sqrt();
  std::cout << TIJ << std::endl;
  std::cout << VIJ << std::endl;
  //
  // 4. A Solver based on Runge-Kutta methods:
  //
  Classical::RungeKuttaSolver solver(H,phaseSpace);
  //  
  // 5. Obtain the solutions:
  //
  x=&solver.equationOf(r[0]);
  y=&solver.equationOf(r[1]);
  px=&solver.equationOf(p[0]);
  py=&solver.equationOf(p[1]);
  // End of the numerical part:---------------------------//
  // ------------------------------------------------------
  // Pop up the main window.
  SoQt::show(&mainwin);

  // Loop until exit.
  SoQt::mainLoop();

  // Clean up resources.
  delete eviewer;
  root->unref();

  return 1;
}


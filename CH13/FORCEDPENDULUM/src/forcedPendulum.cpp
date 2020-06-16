#include <Inventor/Qt/SoQt.h>
#include <Inventor/Qt/viewers/SoQtExaminerViewer.h>
#include <Inventor/Qt/viewers/SoQtPlaneViewer.h>
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
#include "QatGenericFunctions/Sqrt.h"
#include "QatGenericFunctions/Sin.h"
#include "QatGenericFunctions/Cos.h"
#include "QatGenericFunctions/FixedConstant.h"
#include "QatGenericFunctions/RKIntegrator.h"
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

double TimeUnit=1.0;
double O=10.0;
int                step=0;
SoTranslation     *trans=NULL;
SoPointSet        *pointSet=NULL;
SoCoordinate3     *coordinates=NULL;
double            TP=0;
// These three functions describe the coordinate vs time:
const Genfun::AbsFunction *x=NULL, *y=NULL,*z=NULL;


// Invoked at regular intervals to update the electron position:
static void timeSensorCallbackI(void * , SoSensor * )
{
  double t=TP*step++/TimeUnit;
  double X=sin((*x)(t)),Y=(*y)(t),Z=cos((*x)(t));//(*z)(t);
  trans->translation.setValue(X,Y,Z); 
  coordinates->point.set1Value(step,X,Y,Z);
  pointSet->numPoints=step;
}
static void timeSensorCallbackII(void * , SoSensor * )
{
  double t=TP*step++/TimeUnit;
  double R=(*y)(t)+O;
  double Y=0;
  double X=R*sin((*x)(t)),Z=R*cos((*x)(t));//(*z)(t);
  trans->translation.setValue(X,Y,Z); 
  coordinates->point.set1Value(step,X,Y,Z);
  pointSet->numPoints=step;
}

// Main program:

int main (int argc, char * * argv) {

  const std::string usage=std::string(argv[0]) + " [F=val] [b=val] [Omega=val] [TimeUnit=val] [O=val] [pS=val]";
  //
  // Parse the input ------------------------------------------------------
  //
  NumericInput input;
  input.declare("F", "Force ",     1.15);
  input.declare("b", "b ",         0.05);
  input.declare("Omega", "Omega ", 2.0/3.0);
  input.declare("TimeUnit", "Time Unit", 1.0);
  input.declare("O", "Offset ",     0.0);
  input.declare("pS", "Point Size", 1.0);
 try {
    input.optParse(argc, argv);
  }
  catch (std::exception &) {
    std::cout << "Parse error; usage= " << usage << std::endl;
    exit(0);
  }
  
  if (argc!=1) {
    std::cout << "usage: " << usage << std::endl;
    //    exit(0);
  }


  TimeUnit=input.getByName("TimeUnit");
  O=input.getByName("O");
  double F=input.getByName("F");
  double b=input.getByName("b");
  double Omega=input.getByName("Omega");
  double pS    = input.getByName("pS");
  TP=2*M_PI/Omega;

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
  timeSensor->setFunction(O==0.0 ? timeSensorCallbackI : timeSensorCallbackII);
  timeSensor->setBaseTime(SbTime::getTimeOfDay());
  timeSensor->setInterval(0.002f);
  timeSensor->schedule();
  //
  // Make a blue nucleus:
  //
  {
    SoSeparator *sep = new SoSeparator();
    root->addChild(sep);
    
    QColor color("blue");
    SoMaterial *nuclearMaterial = new SoMaterial;
    nuclearMaterial->diffuseColor.setValue(color.red()/255.0, color.green()/255.0, color.blue()/255.0);
    sep->addChild(nuclearMaterial);
    
    SoSphere *sphere = new SoSphere();
    sphere->radius.setValue(0.01);
    sep->addChild(sphere);
  }

  //
  // Make a red electron:
  // 
  {
    SoSeparator *sep = new SoSeparator();
    root->addChild(sep);
    
    QColor color("red");
    SoMaterial *electronMaterial = new SoMaterial;
    electronMaterial->diffuseColor.setValue(color.red()/255.0, color.green()/255.0, color.blue()/255.0);
    sep->addChild(electronMaterial);
    
    trans = new SoTranslation();
    trans->translation.setValue(1,0,0);
    sep->addChild(trans);

    SoSphere *sphere = new SoSphere();
    sphere->radius.setValue(0.05);
    sep->addChild(sphere);
  }

  SoDrawStyle *pds=new SoDrawStyle;
  pds->pointSize=int(pS+0.5);;
  root->addChild(pds);

  coordinates = new SoCoordinate3();
  root->addChild(coordinates);
  pointSet =new SoPointSet;
  root->addChild(pointSet);

  //
  // Make an examiner viewer:
  //
  SoQtFullViewer * eviewer = O==0.0 ? 
    (SoQtFullViewer *) new SoQtExaminerViewer():
    (SoQtFullViewer *) new SoQtPlaneViewer();
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


  Genfun::Variable THETA(0,3);
  Genfun::Variable OMEGA(1,3);
  Genfun::Variable T    (2,3);
  Genfun::RKIntegrator  integrator;
  
  Genfun::GENFUNCTION DTHETADT=OMEGA;
  Genfun::GENFUNCTION DOMEGADT=-Genfun::Sin()(THETA)-b*OMEGA+F*Genfun::Cos()(Omega*T);
  Genfun::GENFUNCTION DTDT    = Genfun::FixedConstant(1.0)%Genfun::FixedConstant(1.0)%Genfun::FixedConstant(1.0);

  integrator.addDiffEquation(&DTHETADT,"THETA",1.0);
  integrator.addDiffEquation(&DOMEGADT,"OMEGA",0.0);
  integrator.addDiffEquation(&DTDT,    "T",    0.0);

  x=integrator.getFunction(THETA.index());
  y=integrator.getFunction(OMEGA.index());
  z=integrator.getFunction(T.index());
  // End of the numerical part:---------------------------//
  // ------------------------------------------------------

  // Pop up the main window.
  SoQt::show(&mainwin);

  // Loop until exit.
  SoQt::mainLoop();

  // Clean up resources.
  //delete eviewer;
  root->unref();

  return 1;
}


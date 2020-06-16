#include "SignalCatcher.h"
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
#include "QatGenericFunctions/RKIntegrator.h"
#include "QatDataAnalysis/OptParse.h"
#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QToolBar>
#include <QAction>
#include <QLabel>
#include <QDoubleSpinBox>
#include <cstdlib>
#include <iostream>
#include <string>
#include <cmath>

double                timeUnit=1.0;
int                   step=0;
SoTranslation        *trans=NULL;
SoPointSet           *pointSet=NULL;
SoCoordinate3        *coordinates=NULL;
SignalCatcher         catcher;


// These three functions describe the coordinate vs time:
const Genfun::AbsFunction *x=NULL, *y=NULL;


// Invoked at regular intervals to update the dsPoint position:
static void timeSensorCallback(void * , SoSensor * )
{
  if (catcher.reset) {
    step=0;
    catcher.reset=0;
  }
  if (catcher.go) {
    double t=step++/5.0;
    t/=timeUnit;
    double X=(*x)(t),Y=(*y)(t), Z=0;
    trans->translation.setValue(X,Y,Z); 
    coordinates->point.set1Value(step,X,Y,Z);
    pointSet->numPoints=step;
  }  
}

// Main program:

int main (int argc, char * * argv) {

  const std::string usage=std::string(argv[0]) + " [E=val] [Theta=val] [Phi=val] [TimeUnit=Val] ";
  //
  // Parse the input ------------------------------------------------------
  //
  NumericInput input;
  input.declare("mu", "mu parameter", 2.5);
  input.declare("X0", "X0 parameter", 1);
  input.declare("V0", "V0 parameter", -6);
  input.declare("TimeUnit", "Unit of Time", 1.0);
  try {
    input.optParse(argc, argv);
  }
  catch (std::exception &) {
    std::cout << "Parse error; usage= " << usage << std::endl;
    exit(0);
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
  timeSensor->setInterval(0.02f);
  timeSensor->schedule();
  //
  // Make a blue dot for the center:
  //
  {
    SoSeparator *sep = new SoSeparator();
    root->addChild(sep);
    
    QColor color("blue");
    SoMaterial *dotMaterial = new SoMaterial;
    dotMaterial->diffuseColor.setValue(color.red()/255.0, color.green()/255.0, color.blue()/255.0);
    sep->addChild(dotMaterial);
    
    SoSphere *sphere = new SoSphere();
    sphere->radius.setValue(0.2);
    sep->addChild(sphere);
  }

  //
  // Make a red dynamical system (ds) dsPoint:
  // 
  {
    SoSeparator *sep = new SoSeparator();
    root->addChild(sep);
    
    QColor color("red");
    SoMaterial *dsPointMaterial = new SoMaterial;
    dsPointMaterial->diffuseColor.setValue(color.red()/255.0, color.green()/255.0, color.blue()/255.0);
    sep->addChild(dsPointMaterial);
    
    trans = new SoTranslation();
    trans->translation.setValue(1,0,0);
    sep->addChild(trans);

    SoSphere *sphere = new SoSphere();
    sphere->radius.setValue(0.2);
    sep->addChild(sphere);
  }

  coordinates = new SoCoordinate3();
  root->addChild(coordinates);
  pointSet =new SoPointSet;
  root->addChild(pointSet);

    
  //
  // Make an examiner viewer:
  //
  SoQtExaminerViewer * eviewer = new SoQtExaminerViewer();
  //  
  // Make a tool bar with a quit button, and set the 
  // Keyboard accelerator:
  //
  QToolBar *toolBar=mainwin.addToolBar("Tools");
  QAction  *quitAction=toolBar->addAction("Quit");
  QAction  *haltAction=toolBar->addAction("Halt");
  QAction  *restartAction=toolBar->addAction("Restart");
  QLabel   *label = new QLabel("mu");
  toolBar->addWidget(label);
  QDoubleSpinBox *muBox=new QDoubleSpinBox();
  toolBar->addWidget(muBox);

  quitAction->setShortcut(QKeySequence("q"));
  haltAction->setShortcut(QKeySequence("h"));
  restartAction->setShortcut(QKeySequence("r"));
  QObject::connect(quitAction, SIGNAL(triggered()), &app, SLOT(quit()));
  QObject::connect(haltAction, SIGNAL(triggered()), &catcher, SLOT(toggleStopGo()));
  QObject::connect(restartAction, SIGNAL(triggered()), &catcher, SLOT(restart()));
  QObject::connect(muBox,SIGNAL(valueChanged(double)), &catcher, SLOT(setMu(double)));
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
  eviewer->getCamera()->position.setValue(0,0,12);
  eviewer->getCamera()->focalDistance=12;
  eviewer->saveHomePosition();
  
  Genfun::RKIntegrator integrator;
  Genfun::Parameter &mu=*integrator.createControlParameter("mu",input.getByName("mu"));
  catcher.mu=&mu;
  muBox->setValue(mu.getValue());

  Genfun::Variable X(0,2);
  Genfun::Variable Y(1,2);
  Genfun::GENFUNCTION DXDT=Y;
  Genfun::GENFUNCTION DYDT=mu*(1-X*X)*Y -X;
  

  integrator.addDiffEquation(&DXDT,"X", input.getByName("X0"));
  integrator.addDiffEquation(&DYDT,"Y", input.getByName("V0"));

  x=integrator.getFunction(X);
  y=integrator.getFunction(Y);

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


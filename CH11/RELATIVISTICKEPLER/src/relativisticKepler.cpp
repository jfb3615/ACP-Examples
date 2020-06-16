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
#include "QatGenericFunctions/Sqrt.h"
#include "QatGenericFunctions/RungeKuttaClassicalSolver.h"
#include "QatGenericFunctions/PhaseSpace.h"
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

double             timeUnit=1.0;
int                step=0;
SoTranslation     *trans=NULL;
SoPointSet        *pointSet=NULL;
SoCoordinate3     *coordinates=NULL;

// These three functions describe the coordinate vs time:
const Genfun::AbsFunction *x=NULL, *y=NULL, *z=NULL;


// Invoked at regular intervals to update the electron position:
static void timeSensorCallback(void * , SoSensor * )
{
  double t=step++/5.0;
  t/=timeUnit;
  double X=(*x)(t),Y=(*y)(t), Z=(*z)(t);
  trans->translation.setValue(X,Y,Z); 
  coordinates->point.set1Value(step,X,Y,Z);
  pointSet->numPoints=step;
}

// Main program:

int main (int argc, char * * argv) {

  const std::string usage=std::string(argv[0]) + " [E=val] [Theta=val] [Phi=val] [TimeUnit=Val] ";
  //
  // Parse the input ------------------------------------------------------
  //
  NumericInput input;
  input.declare("eta", "Boost Factor", 0.66);
  input.declare("Theta", "Polar Angle Electric Field", 0.0);
  input.declare("Phi", "Azimuthal Angle Electric Field", 0.0);
  input.declare("TimeUnit", "Unit of Time", 10.0);
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


  double eta=input.getByName("eta");
  double theta = input.getByName("Theta")*M_PI/180.0;
  double phi   = input.getByName("Phi")*M_PI/180.0;
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
    sphere->radius.setValue(0.1);
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
    sphere->radius.setValue(0.1);
    sep->addChild(sphere);
  }

  // Make an electric field:
  {

    coordinates = new SoCoordinate3();
    root->addChild(coordinates);
    pointSet =new SoPointSet;
    root->addChild(pointSet);

    SoCoordinate3 *eField = new SoCoordinate3();
    eField->point.set1Value(0,0,0,0);
    eField->point.set1Value(1,sin(theta)*cos(phi), sin(theta)*sin(phi), cos(theta));
    root->addChild(eField);
    
    SoDrawStyle *ds= new SoDrawStyle();
    ds->style=SoDrawStyle::LINES;
    ds->lineWidth=3;
    root->addChild(ds);
    
    SoMaterial *eMaterial = new SoMaterial();
    eMaterial->diffuseColor.setValue(1,1,0);
    root->addChild(eMaterial);

    SoLineSet *line = new SoLineSet();
    line->numVertices=2;
    root->addChild(line);
    
  }
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
  // ------------------------------------------------------
  //
  // 1. Phase space is 2x3D:
  //
  Classical::PhaseSpace phaseSpace(3);
  const Classical::PhaseSpace::Component 
    & r=phaseSpace.coordinates(),
    & p=phaseSpace.momenta();
  //
  // 2. Set the initial conditions:
  //
  phaseSpace.start(r[0], 1.0);
  phaseSpace.start(r[1], 0.0);
  phaseSpace.start(r[2], 0.0);
  phaseSpace.start(p[0], 0.0);
  phaseSpace.start(p[1], 1.0);
  phaseSpace.start(p[2], 0.0);
  //
  // 3.  Hamiltonian for the Relativistic Kepler problem:
  //
  Genfun::GENFUNCTION H = 
    Genfun::Sqrt()(p[0]*p[0]+p[1]*p[1]+p[2]*p[2]+(1/eta/eta))-eta/Genfun::Sqrt()(r[0]*r[0]+r[1]*r[1]+r[2]*r[2]);

  //
  // 4. A Solver based on Runge-Kutta methods:
  //
  Classical::RungeKuttaSolver solver(H,phaseSpace);
  //  
  // 5. Obtain the solutions:
  //
  x=&solver.equationOf(r[0]);
  y=&solver.equationOf(r[1]);
  z=&solver.equationOf(r[2]);
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


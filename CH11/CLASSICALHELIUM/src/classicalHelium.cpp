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
#include "QatGenericFunctions/Power.h"
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
SoTranslation     *trans1=NULL;
SoPointSet        *pointSet1=NULL;
SoCoordinate3     *coordinates1=NULL;
SoTranslation     *trans2=NULL;
SoPointSet        *pointSet2=NULL;
SoCoordinate3     *coordinates2=NULL;
SoCoordinate3     *eField=NULL;
const Genfun::AbsFunction *ham=NULL;


// These three functions describe the coordinate vs time:
const Genfun::AbsFunction *x=NULL, *y=NULL, *z=NULL;
const Genfun::AbsFunction *px=NULL, *py=NULL, *pz=NULL;


// Invoked at regular intervals to update the electron position:
static void timeSensorCallback(void * , SoSensor * )
{
  double t=step++/5.0;
  t/=timeUnit;
  double  X=(*x) (t), Y= (*y)(t),  Z= (*z)(t);
  double PX=(*px)(t),PY=(*py)(t), PZ=(*pz)(t);
  trans1->translation.setValue(X,Y,Z); 
  coordinates1->point.set1Value(step,X,Y,Z);
  pointSet1->numPoints=step;
  trans2->translation.setValue(-X,-Y,Z); 
  coordinates2->point.set1Value(step,-X,-Y,Z);
  pointSet2->numPoints=step;
  Genfun::Argument a(6);

  double lx=Y*PZ-Z*PY;
  double ly=Z*PX-X*PZ;
  double lz=X*PY-Y*PX;

  eField->point.set1Value(1,lx,ly,lz);
  
}

// Main program:

int main (int argc, char * * argv) {

  const std::string usage=std::string(argv[0]) + " [E=val] [Theta=val] [Phi=val] [TimeUnit=Val] ";
  //
  // Parse the input ------------------------------------------------------
  //
  NumericInput input;
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
    // Tracks of the red electron:
    coordinates1 = new SoCoordinate3();
    root->addChild(coordinates1);
    pointSet1 =new SoPointSet;
    root->addChild(pointSet1);
    
    SoSeparator *sep = new SoSeparator();
    root->addChild(sep);
    
    QColor color("red");
    SoMaterial *electronMaterial = new SoMaterial;
    electronMaterial->diffuseColor.setValue(color.red()/255.0, color.green()/255.0, color.blue()/255.0);
    sep->addChild(electronMaterial);
    
    trans1 = new SoTranslation();
    trans1->translation.setValue(1,0,0);
    sep->addChild(trans1);

    SoSphere *sphere = new SoSphere();
    sphere->radius.setValue(0.1);
    sep->addChild(sphere);
  }

  //
  // Make a another red electron:
  // 
  {
    // Tracks of the red electron:
    coordinates2= new SoCoordinate3();
    root->addChild(coordinates2);
    pointSet2 =new SoPointSet;
    root->addChild(pointSet2);
    
    SoSeparator *sep = new SoSeparator();
    root->addChild(sep);
    
    QColor color("red");
    SoMaterial *electronMaterial = new SoMaterial;
    electronMaterial->diffuseColor.setValue(color.red()/255.0, color.green()/255.0, color.blue()/255.0);
    sep->addChild(electronMaterial);
    
    trans2 = new SoTranslation();
    trans2->translation.setValue(1,0,0);
    sep->addChild(trans2);

    SoSphere *sphere = new SoSphere();
    sphere->radius.setValue(0.1);
    sep->addChild(sphere);
  }

    // Make an electric field:
  {

    eField = new SoCoordinate3();
    eField->point.set1Value(0,0,0,0);
    eField->point.set1Value(1,0,0,1);
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
  phaseSpace.start(r[0], -1.035021999321);
  phaseSpace.start(r[1],  0.472902831325);
  phaseSpace.start(r[2], -0.355335068341);
  phaseSpace.start(p[0], -0.373230559983);
  phaseSpace.start(p[1], -0.312551998155);
  phaseSpace.start(p[2],  0.728374302946);
  //
  // 3.  Hamiltonian for the Helium atom.  This is for one electron, an
  // effective Hamiltonian. 
  //
  Genfun::GENFUNCTION D1=Genfun::Power(1.0/2.0)(r[0]*r[0]+r[1]*r[1]);
  Genfun::GENFUNCTION D2=Genfun::Power(1.0/2.0)(r[0]*r[0]+r[1]*r[1]+r[2]*r[2]);
  
  Genfun::GENFUNCTION H = p[0]*p[0]/2.0+p[1]*p[1]/2.0+p[2]*p[2]/2.0 + 0.25/D1-2.0/D2;
  ham=&H;
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
  px=&solver.equationOf(p[0]);
  py=&solver.equationOf(p[1]);
  pz=&solver.equationOf(p[2]);
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


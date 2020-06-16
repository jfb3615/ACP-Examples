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
#include "QatGenericFunctions/Sin.h"
#include "QatGenericFunctions/Cos.h"
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
SoCoordinate3     *theString=NULL;
const Genfun::AbsFunction *theta=NULL, *phi=NULL;
const Genfun::AbsFunction *ptheta=NULL, *pphi=NULL;
QColor color[5]={"green", "cyan", "blue", "violet", "orange"};

static void timeSensorCallback(void * , SoSensor * )
{
  double t=step++/5.0;
  t/=timeUnit;
  double Theta=(*theta)(t), Phi=(*phi)(t);
  double X=sin(Theta)*cos(Phi), Z=sin(Theta)*sin(Phi), Y=-cos(Theta);;
  trans->translation.setValue(X,Y,Z); 
  coordinates->point.set1Value(step,X,Y,Z);
  theString->point.set1Value(1,X,Y,Z);
  pointSet->numPoints=step;
}

int main (int argc, char * * argv) {

  // Automatically generated:-------------------------:


  NumericInput input;
  input.declare("TimeUnit", "Unit of Time", 1.0);
  input.declare("pS", "Point Size", 1.0);

  try {
    input.optParse(argc, argv);
  }
  catch (std::exception &) {
    
  }
  
  std::string usage= std::string("usage: ") + argv[0] + " [E=val] [Theta=val] [Phi=val] [TimeUnit=val] [pS=val]"; 
  if (argc!=1) {
    std::cout << usage << std::endl;
  }


  timeUnit=input.getByName("TimeUnit");
  double pS    = input.getByName("pS");


  QApplication     app(argc,argv);
  QMainWindow mainwin;
  mainwin.setMinimumWidth(800);
  mainwin.setMinimumHeight(800);
  SoQt::init(&mainwin);
  
  SoSeparator *root = new SoSeparator;
  root->ref();
  
  // Schedule the update right here: 
  SoTimerSensor * timeSensor = new SoTimerSensor;
  timeSensor->setFunction(timeSensorCallback);
  timeSensor->setBaseTime(SbTime::getTimeOfDay());
  timeSensor->setInterval(0.02f);
  timeSensor->schedule();


  {
    SoSeparator *sep = new SoSeparator();
    root->addChild(sep);
    
    QColor color("blue");
    SoMaterial *molMaterial = new SoMaterial;
    molMaterial->diffuseColor.setValue(color.red()/255.0, color.green()/255.0, color.blue()/255.0);
    sep->addChild(molMaterial);
    
    SoSphere *sphere = new SoSphere();
    sphere->radius.setValue(0.05);
    sep->addChild(sphere);
  }

  
  {
    SoSeparator *sep = new SoSeparator();
    root->addChild(sep);
    
    QColor color("red");
    SoMaterial *molMaterial = new SoMaterial;
    molMaterial->diffuseColor.setValue(color.red()/255.0, color.green()/255.0, color.blue()/255.0);
    sep->addChild(molMaterial);
    
    trans = new SoTranslation();
    trans->translation.setValue(1,0,0);
    sep->addChild(trans);

    SoSphere *sphere = new SoSphere();
    sphere->radius.setValue(0.1);
    sep->addChild(sphere);
  }

  SoDrawStyle *pds=new SoDrawStyle;
  pds->pointSize=int(pS+0.5);;
  root->addChild(pds);

  coordinates = new SoCoordinate3();
  root->addChild(coordinates);
  pointSet =new SoPointSet;
  root->addChild(pointSet);

    {
    theString = new SoCoordinate3();
    theString->point.set1Value(0,0,0,0);
    theString->point.set1Value(1,0,0,1);
    root->addChild(theString);
    
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

    // Use one of the convenient SoQt viewer classes.
  SoQtExaminerViewer * eviewer = new SoQtExaminerViewer();
  

  QToolBar *toolBar=mainwin.addToolBar("Tools");
  QAction  *quitAction=toolBar->addAction("Quit");
  quitAction->setShortcut(QKeySequence("q"));
  QObject::connect(quitAction, SIGNAL(triggered()), &app, SLOT(quit()));

  QVBoxLayout *layout=new QVBoxLayout();
  layout->addWidget(eviewer->getWidget());
  QWidget *widget=new QWidget();
  widget->setLayout(layout);
  mainwin.setCentralWidget(widget);
  eviewer->setSceneGraph(root);

  // Here is the numerical part:---------------------------//
  // ------------------------------------------------------
  Classical::PhaseSpace phaseSpace(2);
  const Classical::PhaseSpace::Component 
    & q=phaseSpace.coordinates(),
    & p=phaseSpace.momenta();
  

  phaseSpace.start(q[0], 0.5);
  phaseSpace.start(q[1], 0.0);
  phaseSpace.start(p[0], 0.0);
  phaseSpace.start(p[1], 0.5);

  Genfun::Sin sin;
  Genfun::Cos cos;
  Genfun::GENFUNCTION H = 0.5*(p[0]*p[0] + p[1]*p[1]/sin(q[0])/sin(q[0])) - cos(q[0]);

  Classical::RungeKuttaSolver solver(H,phaseSpace);
  
  theta=&solver.equationOf(q[0]);
  phi  =&solver.equationOf(q[1]);
  ptheta=&solver.equationOf(p[0]);
  pphi  =&solver.equationOf(p[1]);
  
  //eviewer->getCamera()->pointAt(SbVec3f(0,0,0));
  eviewer->getCamera()->position.setValue(0,0,6);
  eviewer->getCamera()->focalDistance=6;
  eviewer->saveHomePosition();

  // Pop up the main window.
  SoQt::show(&mainwin);

  // Loop until exit.
  SoQt::mainLoop();

  // Clean up resources.
  delete eviewer;
  root->unref();

  return 1;
}


#include <Inventor/Qt/SoQt.h>
#include <Inventor/Qt/viewers/SoQtExaminerViewer.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/sensors/SoTimerSensor.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoLineSet.h>
#include "QatGenericFunctions/RungeKuttaClassicalSolver.h"
#include "QatGenericFunctions/PhaseSpace.h"
#include "QatGenericFunctions/Variable.h"
#include "QatGenericFunctions/Square.h"
#include "QatGenericFunctions/Sigma.h"
#include "QatGenericFunctions/FixedConstant.h"
#include "QatPlotWidgets/PlotView.h"
#include "QatPlotting/PlotStream.h"
#include "QatPlotting/PlotFunction1D.h"
#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QToolBar>
#include <QAction>
#include <cstdlib>
#include <iostream>
#include <string>
#include <cmath>

int               step=0;
SoTranslation    *x    [5];

QColor color[5]={"green", "cyan", "blue", "violet", "orange"};
const Genfun::AbsFunction *fBalls[5]={NULL,NULL,NULL,NULL,NULL};

static void timeSensorCallback(void * , SoSensor * )
{
  double t=step++/5.0;
  for (int i=0;i<5;i++) {
    x[i]->translation.setValue(4.0*(i-2)+0.1*(*fBalls[i])(t),0,0);
  }
}

int main (int argc, char * * argv) {

  // Automatically generated:-------------------------:

  std::string usage= std::string("usage: ") + argv[0]; 
  if (argc!=1) {
    std::cout << usage << std::endl;
  }

  // Here is the numerical part:---------------------------//
  // ------------------------------------------------------
  //
  // 1. Phase space is 2x3D:
  //
  Classical::PhaseSpace phaseSpace(5);
  const Classical::PhaseSpace::Component 
    & r=phaseSpace.coordinates(),
    & p=phaseSpace.momenta();
  //
  // 2. Set the initial conditions:
  //
  phaseSpace.start(r[0], -2.0);
  phaseSpace.start(r[1],  3.0);
  phaseSpace.start(r[2],  0.0);
  phaseSpace.start(r[3], -3.0);
  phaseSpace.start(r[4],  2.0);
  phaseSpace.start(p[0],  0.0);
  phaseSpace.start(p[1],  0.0);
  phaseSpace.start(p[2],  0.0);
  phaseSpace.start(p[3],  0.0);
  phaseSpace.start(p[4],  0.0);
  //
  // 3.  Hamiltonian for the Stark effect:
  //
  const double MM[]={1,3,2,1,2};
  const double KK[]={1,2,2,1};
  Genfun::Square square;
  Genfun::GENFUNCTION H = 
    (1/MM[0]/2.0)*square(p[0])+
    (1/MM[1]/2.0)*square(p[1])+
    (1/MM[2]/2.0)*square(p[2])+
    (1/MM[3]/2.0)*square(p[3])+
    (1/MM[4]/2.0)*square(p[4])+
    (KK[0]/2.0)*square(r[0]-r[1]) +
    (KK[1]/2.0)*square(r[1]-r[2]) +
    (KK[2]/2.0)*square(r[2]-r[3]) +
    (KK[3]/2.0)*square(r[3]-r[4]);
   
  //
  // 4. A Solver based on Runge-Kutta methods:
  //
  Classical::RungeKuttaSolver solver(H,phaseSpace);
  //  
  // 5. Obtain the solutions:
  //
  for (int i=0;i<5;i++) fBalls[i]=&solver.equationOf(r[i]);

  // End of the numerical part:---------------------------//
  // ------------------------------------------------------


  QApplication     app(argc,argv);
  QMainWindow mainwin;
  mainwin.setMinimumWidth(800);
  mainwin.setMinimumHeight(800);
  SoQt::init(&mainwin);
  
  SoSeparator *root = new SoSeparator;
  root->ref();
  
  SoCoordinate3 *coord=new SoCoordinate3();
  coord->point.set1Value(0,-12, 0, 0);
  coord->point.set1Value(1, 12, 0, 0);
  root->addChild(coord);
  SoLineSet *line = new SoLineSet();
  line->numVertices.set1Value(0,2);
  root->addChild(line);
  
  // Schedule the update right here: 
  SoTimerSensor * timeSensor = new SoTimerSensor;
  timeSensor->setFunction(timeSensorCallback);
  timeSensor->setBaseTime(SbTime::getTimeOfDay());
  timeSensor->setInterval(0.1f);
  timeSensor->schedule();

  // Balls size:
  for (int i=0;i<5;i++) {

    SoSeparator *sep = new SoSeparator();
    root->addChild(sep);
    
    SoMaterial *molMaterial = new SoMaterial;
    molMaterial->diffuseColor.setValue(color[i].red()/255.0, color[i].green()/255.0, color[i].blue()/255.0);
    sep->addChild(molMaterial);

    x[i] = new SoTranslation;
    x[i]->translation.setValue(4.0*(i-2),0,0);
    sep->addChild(x[i]);
    
    SoSphere *sphere = new SoSphere();
    sphere->radius.setValue(pow(MM[i],1.0/3.0));
    sep->addChild(sphere);
    
  }
    
  // Use one of the convenient SoQt viewer classes.
  SoQtExaminerViewer * eviewer = new SoQtExaminerViewer();
  eviewer->setDecoration(false);
  PRectF rect;
  rect.setXmin(0.0);
  rect.setXmax(20.0);
  rect.setYmin(-12);
  rect.setYmax(12.0);

  std::vector<PlotFunction1D *> pFunctions;
  Genfun::Sigma cmMotion;
  double sumMass=0.0;
  PlotView *view = new PlotView(rect);
  for (int i=0;i<5;i++) {
    Genfun::Sigma sigma;
    sigma.accumulate(Genfun::FixedConstant(4*(i-2)));
    Genfun::GENFUNCTION F = *fBalls[i];
    sigma.accumulate(F);
    PlotFunction1D *p=new PlotFunction1D(sigma);
    pFunctions.push_back(p);
    {
      PlotFunction1D::Properties prop;
      prop.pen.setColor(color[i]);
      prop.pen.setWidth(2);
      p->setProperties(prop);
    }
    view->add(p);
    cmMotion.accumulate(MM[i]*sigma);
    sumMass+= MM[i];
  }
  PlotFunction1D *pCM=new PlotFunction1D(cmMotion/sumMass);
  pFunctions.push_back(pCM);
  {
    PlotFunction1D::Properties prop;
    prop.pen.setColor("red");
    prop.pen.setWidth(3);
    pCM->setProperties(prop);
  }
  view->add(pCM);


  PlotStream titleStream(view->titleTextEdit());
  titleStream << PlotStream::Clear()
	      << PlotStream::Center()
	      << PlotStream::Family("Sans Serif") 
	      << PlotStream::Size(18)
	      << "Five oscillating balls"
	      << PlotStream::EndP();

  PlotStream tStream(view->xLabelTextEdit());
  tStream << PlotStream::Clear()
	  << PlotStream::Center()
	  << PlotStream::Family("Sans Serif") 
	  << PlotStream::Size(18)
	  << "t"
	  << PlotStream::EndP();

  PlotStream xStream(view->yLabelTextEdit());
  xStream << PlotStream::Clear()
	  << PlotStream::Center()
	  << PlotStream::Family("Sans Serif") 
	  << PlotStream::Size(18)
	  << "x"
	  << PlotStream::EndP();

  QVBoxLayout *layout=new QVBoxLayout();
  layout->addWidget(eviewer->getWidget());
  layout->addWidget(view);
  QWidget *widget=new QWidget();
  widget->setLayout(layout);
  mainwin.setCentralWidget(widget);
  eviewer->setSceneGraph(root);

  eviewer->viewAll();
  eviewer->show();
  
  QToolBar *toolBar=mainwin.addToolBar("Tools");
  QAction  *nextAction=toolBar->addAction("Next");
  nextAction->setShortcut(QKeySequence("n"));
  QObject::connect(nextAction, SIGNAL(triggered()), &app, SLOT(quit()));
  
  // Pop up the main window.
  SoQt::show(&mainwin);
  // Loop until exit.
  SoQt::mainLoop();

  for (PlotFunction1D *p : pFunctions) delete p;
  
  // Clean up resources.
  delete eviewer;
  root->unref();

  return 1;
}


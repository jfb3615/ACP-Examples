#include "Animator.h"
#include <Inventor/Qt/SoQt.h>
#include <Inventor/Qt/viewers/SoQtExaminerViewer.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoCamera.h>
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


// Main program:

int main (int argc, char * * argv) {

#ifndef __APPLE__
  setenv("QT_QPA_PLATFORM","xcb",0);
#endif
  
  const std::string usage=std::string(argv[0]) + " [eta=val] [Theta=val] [Phi=val] [TimeUnit=val] [PointSize=val] ";
  //
  // Parse the input ------------------------------------------------------
  //
  NumericInput input;
  input.declare("eta", "Boost Factor", 0.66);
  input.declare("Theta", "Polar Angle Electric Field", 0.0);
  input.declare("Phi", "Azimuthal Angle Electric Field", 0.0);
  input.declare("TimeUnit", "Unit of Time", 10.0);
  input.declare("PointSize", "Size of points, trajectory trail", 3);
  try {
    input.optParse(argc, argv);
  }
  catch (std::exception &) {
    std::cout << "Parse error; usage= " << usage << std::endl;
    exit(0);
  }
  
  // Check for unrecognized argument
  if (argc!=1) {
    std::cout << "usage: " << usage << std::endl;
    exit(0);
  }

  double eta=input.getByName("eta");
  double theta   = input.getByName("Theta")*M_PI/180.0;
  double phi     = input.getByName("Phi")*M_PI/180.0;
  double timeUnit= input.getByName("TimeUnit");
  unsigned int pS= input.getByName<unsigned int> ("PointSize");
  //-------------------------------------------------------------------------
  //
  // Initialize graphics:
  //
  QApplication     app(argc,argv);
  QMainWindow mainwin;
  mainwin.setMinimumWidth(800);
  mainwin.setMinimumHeight(800);
  SoQt::init(&mainwin);
  
  // Animator handles the time evolution:
  Animator animator;
  animator.setPointSize(pS);
  animator.setTimeUnit(timeUnit);

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
  eviewer->setSceneGraph(animator.getSceneGraph());
  //
  // Position the camera (overriding some inconvenient defaults)
  //
  eviewer->getCamera()->position.setValue(0,0,6);
  eviewer->getCamera()->focalDistance=6;
  eviewer->saveHomePosition();


  // Here is the numerical part:---------------------------//
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
  animator.setXFunction(solver.equationOf(r[0]));
  animator.setYFunction(solver.equationOf(r[1]));
  animator.setZFunction(solver.equationOf(r[2]));
  // End of the numerical part:---------------------------//
  // ------------------------------------------------------

  // Pop up the main window.
  SoQt::show(&mainwin);

  // Loop until exit.
  SoQt::mainLoop();

  // Clean up resources.
  delete eviewer;
  return 1;
}


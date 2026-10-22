#include "Animator.h"
#include <Inventor/Qt/SoQt.h>
#include <Inventor/Qt/viewers/SoQtExaminerViewer.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoCamera.h>
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


int main (int argc, char * * argv) {

  std::string usage= std::string("usage: ") + argv[0] + " [E=val] [Theta=val] [Phi=val] [TimeUnit=val] [pS=val]"; 

  
#ifndef __APPLE__
  setenv("QT_QPA_PLATFORM","xcb",0);
#endif
  
  // Parse the command line:
  NumericInput input;
  input.declare("TimeUnit", "Unit of Time, increasing slows down", 1.0);
  input.declare("pS",       "Point Size",                            1);

  try {
    input.optParse(argc, argv);
  }
  catch (std::exception & e) {
    std::cout << e.what()      << std::endl;
    std::cout << usage << std::endl;
    std::cout << input.usage() << std::endl;
    return 1;
  }
  
  // check for unrecognized input:
  if (argc!=1) {
    std::cout << usage << std::endl;
    return 0;
  }

  // get timeUnit and point size values:
  double timeUnit     = input.getByName("TimeUnit");
  unsigned int pS     = input.getByName<unsigned int>("pS");

  // Start application, window, toolbar, buttons, etc.
  QApplication     app(argc,argv);
  QMainWindow mainwin;
  mainwin.setMinimumWidth(800);
  mainwin.setMinimumHeight(800);
  
  QToolBar *toolBar=mainwin.addToolBar("Tools");
  QAction  *quitAction=toolBar->addAction("Quit");

  quitAction->setShortcut(QKeySequence("q"));
  QObject::connect(quitAction, &QAction::triggered, &app, &QApplication::quit);

  // Initialize SoQt:
  SoQt::init(&mainwin);
  
  // The scene management and updates are handled in the animator class:
  Animator animator;
  animator.setPointSize(pS);
  animator.setTimeUnit(timeUnit);

  // Make a viewer:
  SoQtExaminerViewer * eviewer = new SoQtExaminerViewer();
  eviewer->setDoubleBuffer(false);

  // Place the viewer in the widget hierarchy:
  QVBoxLayout *layout=new QVBoxLayout();
  layout->addWidget(eviewer->getWidget());
  QWidget *widget=new QWidget();
  widget->setLayout(layout);
  mainwin.setCentralWidget(widget);
  eviewer->setSceneGraph(animator.getSceneGraph());

  // Here is the numerical part:--------------------------------------------------------------------//
  // -----------------------------------------------------------------------------------------------//
  Classical::PhaseSpace phaseSpace(2);                                                              //
  const Classical::PhaseSpace::Component                                                            // 
    & q=phaseSpace.coordinates(),                                                                   //
    & p=phaseSpace.momenta();                                                                       //
  //                                                                                                //
  phaseSpace.start(q[0], 0.5);                                                                      //
  phaseSpace.start(q[1], 0.0);                                                                      //
  phaseSpace.start(p[0], 0.0);                                                                      //
  phaseSpace.start(p[1], 0.5);                                                                      //
  // Define the Hamiltonian for the Spherical Pendulum                                              //
  Genfun::Sin sin;                                                                                  //
  Genfun::Cos cos;                                                                                  //
  Genfun::GENFUNCTION H = 0.5*(p[0]*p[0] + p[1]*p[1]/sin(q[0])/sin(q[0])) - cos(q[0]);              //
  // Pass it along to the solver:                                                                   //
  Classical::RungeKuttaSolver solver(H,phaseSpace);                                                 //
  // Get the solution                                                                               //
  animator.setThetaFunction(solver.equationOf(q[0]));                                               //
  animator.setPhiFunction  (solver.equationOf(q[1]));                                               //
  //                                                                                                //
  //=================================================================================================
  
  eviewer->getCamera()->position.setValue(0,0,6);
  eviewer->getCamera()->focalDistance=6;
  eviewer->saveHomePosition();

  // Pop up the main window.
  SoQt::show(&mainwin);

  // Loop until exit.
  SoQt::mainLoop();

  // Clean up resources.
  delete eviewer;

  return 1;
}


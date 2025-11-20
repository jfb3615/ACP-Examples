#include <iostream>
#include "Eigen/Dense"
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
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/nodes/SoVertexProperty.h>
#include <Inventor/sensors/SoTimerSensor.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoBaseColor.h>

#include <QApplication>
#include <QMainWindow>
#include <QLayout>
#include <QAction>
#include <QToolBar>
//define constants and parameters


SoPointSet *pset=NULL;
PlotProfile totalEnergyProf,moleculeEnergyProf,workDoneProf, potentialEnergyProf;
PlotView *pv_g{nullptr};

// These two properties contain information that
// is to be updated (positions) at each time step:
//
// Position of the molecules:
SoVertexProperty *property=NULL;
//
// Position of the piston:
SoTranslation *translation=NULL;


std::vector<SoTranslation*> sphereTranslations;
using namespace std;
using namespace Eigen;


//
// This is a random variate generator:
//


#include "MolecularModel.h"
#include "IdealGasModel.h"
#include "PeriodicIdeal.h"
#include "ArgonLennardJones.h"
#include "HardSpheres.h"

AbsModel *model{nullptr};
IdealGasModel *idealGasModel{nullptr};
HardSpheres   *hardSpheresModel{nullptr};
ArgonLennardJones *lennardJonesModel{nullptr};
void
timeSensorCallback(void * , SoSensor * )
{
  //
  // Call time evolution to update position array x[]:
  //
  static unsigned int k{0};
  model->takeStep(0.01);
  double t=model->getTime();
  if (!(k++ % 10)) {
    workDoneProf.addPoint(t,model->getWorkOnEnvironment());
    moleculeEnergyProf.addPoint(t,model->getKineticEnergy());
    potentialEnergyProf.addPoint(t,model->getPotentialEnergy());
    totalEnergyProf.addPoint(t,model->getTotalEnergy()+model->getWorkOnEnvironment());
    pv_g->recreate(); // refresh the view. 
  }

  //
  // Turn off the notification mechanism because extensive
  // editing will now take place:
  //
  pset->vertexProperty.enableNotify(false);
  //
  // Set the position of all the points in the point set,
  // which represent molecules:
  //
  for(unsigned int i=0;i<model->getNumParticles();i++) 
    {
      const Vector3d &x=model->getPosition(i);
      property->vertex.set1Value(i,x.y(),x.z(),x.x());
      if (hardSpheresModel || lennardJonesModel) sphereTranslations[i]->translation.setValue(x.y(),x.z(), x.x());
    }
  //
  // Turn notification back on:
  //
  pset->vertexProperty.enableNotify(true);
  //
  // Force an update:
  //
  pset->vertexProperty.touch();
  //
  // Modify the position of the piston:
  //
  if (idealGasModel) translation->translation.setValue(0,idealGasModel->getPistonZ(),0);
}


int main(int argc, char **argv)
{
#ifndef __APPLE__
  setenv("QT_QPA_PLATFORM","xcb",0);
#endif  

  double E{1000.0};
  double N{2000.0};
  double a{0.01};
  double ePs{1.0}; // Interaction strength ("Leonard-Jones");
  double system=1;
  NumericInput input;
  input.declare("E",   "Energy of system",    E);
  input.declare("N",   "Number of Particles", N);
  input.declare("a",   "radius of sphere",    a);
  input.declare("EPS", "interaction strength",ePs);
  input.declare("system", "1=piston,2=periodic, 3=hardspheres,4=lennardjones", system); 
  try {
    input.optParse(argc,argv);
  }
  catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    std::cerr << input.usage() << std::endl;
    return 1;
  }

  E=input.getByName<double> ("E");
  N=input.getByName<unsigned int> ("N");
  a=input.getByName<double> ("a");
  ePs=input.getByName<double> ("EPS");
  system=input.getByName<unsigned int> ("system");

  if (system==1) {
    idealGasModel=new IdealGasModel(N,1.0,100.0,E);
    model=idealGasModel;
  }
  else if (system==2) {
    model=new PeriodicIdeal(N,1.0,E);
  }
  else if (system==3) {
    hardSpheresModel=new HardSpheres(N,1.0,a,2.0);
    model=hardSpheresModel;
  }
  else if (system==4) {
    lennardJonesModel =new ArgonLennardJones(N,1.0,a, ePs, E);
    model=lennardJonesModel;
  }
  //---------------------------------------------------------
  // Now that we have reboundCollection, start visualizing!
  QApplication app(argc, argv);

  MultipleViewWindow mainwin;
  mainwin.resize(600,600);
  SoQt::init(&mainwin);

  QToolBar *toolBar=mainwin.addToolBar("Tools");
  QAction  *quitAction=toolBar->addAction("Quit");
  
  quitAction->setShortcut(QKeySequence("q"));
  
  QObject::connect(quitAction, SIGNAL(triggered()), &app, SLOT(quit()));
  

  
  SoSeparator *root = new SoSeparator;
  root->ref();
  
  // Schedule the update right here: 
  SoTimerSensor * timeSensor = new SoTimerSensor;
  timeSensor->setFunction(timeSensorCallback);
  timeSensor->setBaseTime(SbTime::getTimeOfDay());
  timeSensor->setInterval(0.005f);
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

  SoMaterial *molMaterial = new SoMaterial;
  molMaterial->diffuseColor.setValue(1.0, 0.0, 0.0);
  root->addChild(molMaterial);

  pset = new SoPointSet;
  property = new SoVertexProperty;
  pset->vertexProperty=property;
  root->addChild(pset);

  // Ideal Gas:  set up a cylinder with movable piston. 
  if (idealGasModel) {
    
    SoMaterial *cylMaterial = new SoMaterial;
    cylMaterial->diffuseColor.setValue(0.0, 0.0, 1.0);
    cylMaterial->transparency=0.5;
    root->addChild(cylMaterial);
    
    SoDrawStyle *cylDrawStyle=new SoDrawStyle();
    root->addChild(cylDrawStyle);
  
    
    SoCylinder *cyl=new SoCylinder;
    cyl->height=0.01;
    cyl->radius=1.0;
    root->addChild(cyl);
    
    translation=new SoTranslation;
    translation->translation.setValue(0,idealGasModel->getPistonZ(),0);
    root->addChild(translation);
    root->addChild(cyl);
    
  }
  else if (hardSpheresModel || lennardJonesModel) {
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
    for (int i=0;i<N;i++) {
      SoSeparator *sep=new SoSeparator;
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

  PlotView energyView({0.0,20.0,0.0, 1200.0});
  energyView.add(&workDoneProf);
  energyView.add(&moleculeEnergyProf);
  energyView.add(&potentialEnergyProf);
  energyView.add(&totalEnergyProf);
  mainwin.add(&energyView,"Energy");
  pv_g=&energyView;  

  {
    PlotProfile::Properties prop;
    prop.pen.setStyle(Qt::NoPen);
    prop.brush.setStyle(Qt::SolidPattern);
    prop.brush.setColor("blue");
    workDoneProf.setProperties(prop);
    prop.brush.setColor("red");
    moleculeEnergyProf.setProperties(prop);
    prop.brush.setColor("green");
    potentialEnergyProf.setProperties(prop);
    prop.brush.setColor("black");
    totalEnergyProf.setProperties(prop);
  }

  PlotStream titleStream(energyView.titleTextEdit());
  titleStream << PlotStream::Clear()
	      << PlotStream::Center() 
	      << PlotStream::Family("Arial") 
	      << PlotStream::Size(16)
	      << "Adiabatic expansion of a gas"
	      << PlotStream::EndP();
  
  
  PlotStream xLabelStream(energyView.xLabelTextEdit());
  xLabelStream << PlotStream::Clear()
	       << PlotStream::Center()
	       << PlotStream::Family("Arial")
	       << PlotStream::Size(16)
	       << "time"
	       << PlotStream::EndP();
  
  PlotStream yLabelStream(energyView.yLabelTextEdit());
  yLabelStream << PlotStream::Clear()
	       << PlotStream::Center()
	       << PlotStream::Family("Arial")
	       << PlotStream::Size(16)
	       << "Energy"
	       << PlotStream::EndP();
  
  PlotStream statBoxStream(energyView.statTextEdit());
  statBoxStream << PlotStream::Clear()
		<< PlotStream::Left()
		<< PlotStream::Family("Arial")
		<< PlotStream::Size(19)
		<< PlotStream::Color("blue")
    		<< "･ external work\n" 
		<< PlotStream::Color("red")
		<< "･ kinetic energy\n"
		<< PlotStream::Color("green")
		<< "･ potential energy\n"
		<< PlotStream::Color("black")
		<< "･ total"
		<< PlotStream::EndP();
  

  

  std::cout << "Kinetic"   << model->getKineticEnergy() << std::endl;
  std::cout << "Potential" <<  model->getPotentialEnergy() << std::endl;
  
  // Pop up the main window.
  SoQt::show(&mainwin);
  // Loop until exit.
  SoQt::mainLoop();

  // Clean up resources.
  delete eviewer;
  root->unref();



}

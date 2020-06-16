#include "QatDataAnalysis/OptParse.h"
#include <Inventor/Qt/SoQt.h>
#include <Inventor/Qt/viewers/SoQtExaminerViewer.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>
#include <Inventor/nodes/SoRotation.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/nodes/SoVertexProperty.h>
#include "Psi2Hydrogen.h"
#include <Inventor/sensors/SoTimerSensor.h>
#include "Eigen/Dense"
#include <iostream>
#include <vector>
#include <sstream>
#include <random>

using namespace std;

#define RBD_EPSILON 1E-4

SoVertexProperty *property = NULL;
SoPointSet       *set=NULL;
unsigned int N=1,L=0,M=0;
unsigned int NPOINTS=10000;
double fSigma=1.0;

static Eigen::VectorXd vState(3);
static double vProb=0;


class HydrogenAtomMarkovChain {


public:
  
  HydrogenAtomMarkovChain(int n, int l, int m, double sigma=1.0):psi(n,l,m),
								 gauss(0,sigma)
  {
  }

  double move(Eigen::VectorXd & v, double prob=0.0) {
    
    Eigen::VectorXd vProp=v;
    for (int d=0;d<vProp.rows();d++) {
      vProp[d]+=gauss(engine);
    }
    
    // The argument to the hydrogen wavefunction class is in spherical 
    // polar coordinates:
    Genfun::Argument r(3);
    r[0]=sqrt(vProp[0]*vProp[0]+vProp[1]*vProp[1]+vProp[2]*vProp[2]);
    r[1]=vProp[2]/r[0];
    r[2]=atan2(vProp[1],vProp[0]);
    
    
    double newProb=psi(r);
    if (newProb>prob) {
      v=vProp;
      return newProb;
    }
    else {
      if (flat(engine) < newProb/prob) {
	v=vProp;
	return newProb;
      }
      else {
	return prob;
      }
    }
  }
  
private:
  
  Genfun::Psi2Hydrogen psi;   // the Hydrogen PDF
  std::mt19937                              engine;
  std::normal_distribution<double>          gauss;
  std::uniform_real_distribution<double>    flat;
};



static void
timeSensorCallback(void * , SoSensor * )
{
  static HydrogenAtomMarkovChain chain(N,L,M,fSigma*N);

  unsigned int NTOT=2000;
  static unsigned int count=0;
  if (count>=NPOINTS) return;

  set->vertexProperty.enableNotify(false);
  for(unsigned int i=0;i<NTOT;i++) 
  {
  
    vProb=chain.move(vState,vProb);
    property->vertex.set1Value(count++,vState[0],vState[2],vState[1]);
  }
  set->vertexProperty.enableNotify(true);
  set->vertexProperty.touch();
}


int main (int argc, char ** argv) {
  std::string usage = std::string("usage: ") + argv[0] + "[NPOINTS=val{def=10000}] [N=val{def=0}] [L=val{def=0}] [M=val{def=0}] [fSigma=val{def=1.0}";
  NumericInput numeric;
  numeric.declare("NPOINTS", "Number of points in the Markov Chain", 100000);
  numeric.declare("N",       "Principal Quantum Number", 1);
  numeric.declare("L",       "Angular Momentum Quantum Number", 0);
  numeric.declare("M",       "Magnetic Quantum Number", 0);
  numeric.declare("fSigma",  "Under/over scale the proposal distribution", 0.05);
  numeric.optParse(argc, argv);
  vState(0)=30;
  vState(1)=0;
  vState(2)=0;

  
  if (argc>1) {
    std::cout << usage << std::endl;
    exit(0);
  }
  NPOINTS = (unsigned int)(0.5+numeric.getByName("NPOINTS"));
  N      = (unsigned int)(0.5+numeric.getByName("N"));
  L      = (unsigned int)(0.5+numeric.getByName("L"));
  M      = (unsigned int)(0.5+numeric.getByName("M"));
  fSigma     = numeric.getByName("fSigma");  

  Eigen::VectorXd v(3);
  v[0]=0;
  v[1]=0;
  v[2]=0;



  //---------------------------------------------------------
  // Now that we have reboundCollection, start visualizing!

  QWidget * mainwin = SoQt::init("Molecular Collisions");
  
  SoSeparator *root = new SoSeparator;
  root->ref();
  
    
  SoMaterial *cubeMaterial = new SoMaterial;
  cubeMaterial->diffuseColor.setValue(0.0, 0.0, 1.0);
  root->addChild(cubeMaterial);

  SoDrawStyle *cubeDrawStyle=new SoDrawStyle();
  cubeDrawStyle->style=SoDrawStyle::LINES;
  root->addChild(cubeDrawStyle);

  SoLightModel *cubeLightModel = new SoLightModel;
  cubeLightModel->model=SoLightModel::BASE_COLOR;
  root->addChild(cubeLightModel);

  SoCube *cube = new SoCube();
  cube->width=30;
  cube->height=30;
  cube->depth=30;
  root->addChild(cube);

  SoMaterial *walkMaterial = new SoMaterial;
  walkMaterial->diffuseColor.setValue(1.0, 0.0, 0.0);
  root->addChild(walkMaterial);

  property = new SoVertexProperty();
  SoTimerSensor * timeSensor = new SoTimerSensor;
  timeSensor->setFunction(timeSensorCallback);
  timeSensor->setBaseTime(SbTime::getTimeOfDay());
  timeSensor->setInterval(1.0f);
  timeSensor->schedule();

  set = new SoPointSet;
  set->vertexProperty=property;
  root->addChild(set);
    
  // Use one of the convenient SoQt viewer classes.
  SoQtExaminerViewer * eviewer = new SoQtExaminerViewer(mainwin);
  eviewer->setTransparencyType(SoGLRenderAction::SCREEN_DOOR);
  eviewer->setSceneGraph(root);
  eviewer->show();
  
  // Pop up the main window.
  SoQt::show(mainwin);
  // Loop until exit.
  SoQt::mainLoop();

  // Clean up resources.
  delete eviewer;
  root->unref();
  



  return 0;
}






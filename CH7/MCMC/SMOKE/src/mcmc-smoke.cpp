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
#include "Eigen/Dense"
#include "QatGenericFunctions/AbsFunction.h"
#include <Inventor/sensors/SoTimerSensor.h>
#include <iostream>
#include <vector>
#include <random>

using namespace std;

#define RBD_EPSILON 1E-4

// The following is a class that carries out Markov Chain Monte Carlo according
// to probability density functions 

SoVertexProperty *property = NULL;


class MarkovChain {


public:
  
  MarkovChain():
    seed1(999),
    seed2(345)
  {
  }

  double move(Eigen::VectorXd & v, double prob=0.0) {
    
    Eigen::VectorXd vProp=v;
    for (int d=0;d<vProp.rows();d++) {
      vProp[d]+=gauss(engine);
    }
    
    double newProb=1;
    if (fabs(vProp[0])>15) return 0;
    if (fabs(vProp[1])>15) return 0;
    if (fabs(vProp[2])>15) return 0;
    if (newProb>=prob) {
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

  unsigned int seed1;
  unsigned int seed2;
  std::mt19937 engine;
  std::normal_distribution<double> gauss;
  std::uniform_real_distribution<double> flat;

};

static void
timeSensorCallback(void *, SoSensor *)
{
  static Eigen::VectorXd v(3);
  
  static MarkovChain chain;

  unsigned int NTOT=100;
  static unsigned int count=0;
  for(unsigned int i=0;i<NTOT;i++) 
  {
    chain.move(v);
    property->vertex.set1Value(count++,v[0],v[1],v[2]);
  }  
}

int main (int , char **) {
  

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

  SoDrawStyle *walkDrawStyle=new SoDrawStyle();
  walkDrawStyle->style=SoDrawStyle::LINES;
  root->addChild(walkDrawStyle);

  
  SoMaterial *walkMaterial = new SoMaterial;
  walkMaterial->diffuseColor.setValue(1.0, 0.0, 0.0);
  root->addChild(walkMaterial);

  property = new SoVertexProperty();


  SoTimerSensor * timeSensor = new SoTimerSensor;
  timeSensor->setFunction(timeSensorCallback);
  timeSensor->setBaseTime(SbTime::getTimeOfDay());
  timeSensor->setInterval(0.1f);
  timeSensor->schedule();
  



  SoLineSet *set = new SoLineSet;
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






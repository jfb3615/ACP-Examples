#include "Animator.h"
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSphere.h>
#include <QColor>
Animator::~Animator() {
    root->unref();
}

void Animator::setPointSize(unsigned int ps) {
  pS=ps;
}

void Animator::setTimeUnit(double tu) {
  timeUnit=tu;
}

void Animator::setThetaFunction(const Genfun::GENFUNCTION tFunction) {
  theta=&tFunction;
}

void Animator::setPhiFunction(const Genfun::GENFUNCTION pFunction) {
  phi=&pFunction;
}

Animator::Animator() {
  
  // Set up a timer.
  timeSensor.setFunction(&Animator::timeSensorCallback);
  timeSensor.setData(this);
  timeSensor.setBaseTime(SbTime::getTimeOfDay());
  timeSensor.setInterval(0.02f);
  timeSensor.schedule();
  
  root = new SoSeparator;
  root->ref();

  // Construct the pendulum fulcrum:
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
  
  // Construct the pendulum bob:
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

  // Construct the trajectory trail out of points:
  SoDrawStyle *pds=new SoDrawStyle;
  pds->pointSize=pS;
  root->addChild(pds);
  
  coordinates = new SoCoordinate3();
  root->addChild(coordinates);
  pointSet =new SoPointSet;
  root->addChild(pointSet);

  // Construct the massless rod and/or string:
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
  
}

SoSeparator *Animator::getSceneGraph() const {
  return root;
}

void Animator::timeSensorCallback(void * data , SoSensor * ) {
  
  Animator *This=(Animator *) data;
  double t=This->step++/5.0;
  t/=This->timeUnit;
  double Theta=(*This->theta)(t), Phi=(*This->phi)(t);
  double X=sin(Theta)*cos(Phi), Z=sin(Theta)*sin(Phi), Y=-cos(Theta);;
  This->trans->translation.setValue(X,Y,Z); 
  This->coordinates->point.set1Value(This->step,X,Y,Z);
  This->theString->point.set1Value(1,X,Y,Z);
  This->pointSet->numPoints=This->step;
}
  

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

void Animator::setXFunction(const Genfun::GENFUNCTION xFunction) {
  x=&xFunction;
}

void Animator::setYFunction(const Genfun::GENFUNCTION yFunction) {
  y=&yFunction;
}

void Animator::setZFunction(const Genfun::GENFUNCTION zFunction) {
  z=&zFunction;
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
    eField->point.set1Value(1,0,0,1.0);
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
}

SoSeparator *Animator::getSceneGraph() const {
  return root;
}

void Animator::timeSensorCallback(void * data , SoSensor * ) {
  
  Animator *This=(Animator *) data;
  double t=This->step++/5.0;
  t/=This->timeUnit;

  double X=(*This->x)(t);
  double Y=(*This->y)(t);
  double Z=(*This->z)(t);
  This->trans->translation.setValue(X,Y,Z); 
  This->coordinates->point.set1Value(This->step,X,Y,Z);
  This->pointSet->numPoints=This->step;
}
  

#include <Inventor/Qt/SoQt.h>
#include <Inventor/Qt/viewers/SoQtExaminerViewer.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/nodes/SoVertexProperty.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoTexture2.h>
#include <Inventor/sensors/SoTimerSensor.h>
#include <QApplication>
#include <QWidget>
#include <random>
#include <stdexcept>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <cmath>




SoTranslation *planetTranslation=NULL;
double tPhi=0.0;
void callback(void *, SoSensor *) {
  static const double R=10;
  planetTranslation->translation.setValue(R*cos(tPhi), R*sin(tPhi), 0);
  tPhi+=0.01;
}

SoSeparator *makeOrbit(double a) {

  SoSeparator *sep=new SoSeparator();

  SoCoordinate3 *coords=new SoCoordinate3;
  unsigned int NPOINTS=100;
  for (unsigned int i=0;i<NPOINTS;i++) {
    double phi=2*M_PI*i/double(NPOINTS);
    coords->point.set1Value(i,a*cos(phi),a*sin(phi),0);
  }
  sep->addChild(coords);
  SoLineSet *lineSet=new SoLineSet;
  lineSet->numVertices=NPOINTS;
  sep->addChild(lineSet);

  return sep;
}




int main(int argc, char ** argv)
{

  QApplication app(argc, argv);
  QWidget mainwin;
  SoQt::init(&mainwin);

  // The root of a scene graph
  SoSeparator *root = new SoSeparator;
  root->ref();
  
  SoQtExaminerViewer * eviewer = new SoQtExaminerViewer(&mainwin);
  eviewer->setSceneGraph(root);
  eviewer->show();

  static const double a=10.0;
  root->addChild(makeOrbit(a));

  {
    SoTexture2 *txt=new SoTexture2;
    txt->filename="photo.jpg";
    root->addChild(txt);
  }

  
  SoSphere *sphere=new SoSphere;
  root->addChild(sphere);

  {
    SoTexture2 *txt=new SoTexture2;
    txt->filename="photo2.jpg";
    root->addChild(txt);
  }
  
  // Schedule the update right here: 
  SoTimerSensor * timerSensor = new SoTimerSensor;
  timerSensor->setFunction(callback);
  timerSensor->setInterval(0.01);
  timerSensor->schedule();
  

  SoSeparator *planetSep=new SoSeparator;
  SoSphere    *planet = new SoSphere;
  planetTranslation = new SoTranslation();
  planetTranslation->translation.setValue(a, 0, 0);
  planetSep->addChild(planetTranslation);
  planetSep->addChild(planet);
  root->addChild(planetSep);
  
  // Pop up the main window.
  SoQt::show(&mainwin);

  eviewer->viewAll();
  
  // Loop until exit.
  SoQt::mainLoop();

  // Clean up resources.
  delete eviewer;
  root->unref();
}

#include <Inventor/Qt/SoQt.h>
#include <Inventor/Qt/viewers/SoQtExaminerViewer.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/SbLinear.h>
#include <Inventor/nodes/SoRotation.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoVertexProperty.h>
#include <QApplication>
#include <QWidget>
#include "QatGenericFunctions/Variable.h"
#include "QatGenericFunctions/RKIntegrator.h"
#include <iostream>
#include <vector>

using namespace Genfun;
using namespace std;

int main (int argc, char **argv) {
  
  double s=10;
  double b=8.0/3.0;
  double r = 28.;
 
  Variable X(0,3),Y(1,3),Z(2,3);

  GENFUNCTION DXDT = s*(Y-X); 
  GENFUNCTION DYDT = r*X -Y -X*Z;
  GENFUNCTION DZDT = X*Y - b*Z;
  
  RKIntegrator integrator;
  
  Parameter *x0=integrator.addDiffEquation(&DXDT, "X0", 2, -10, 10);
  integrator.addDiffEquation(&DYDT, "Y0", 5, -10, 10);
  integrator.addDiffEquation(&DZDT, "Z0", 5, -10, 10);
  
  const AbsFunction   *xt = integrator.getFunction(X);
  const AbsFunction   *yt = integrator.getFunction(Y);
  const AbsFunction   *zt = integrator.getFunction(Z);
  
  x0->setValue(30);
  //---------------------------------------------------------
  // Now that we have reboundCollection, start visualizing!

  QApplication app(argc, argv);
  QWidget mainwin;
  mainwin.setMinimumWidth(800);
  mainwin.setMinimumHeight(800);
  SoQt::init(&mainwin);
  
  SoSeparator *root = new SoSeparator;
  root->ref();


  SoMaterial *blue= new SoMaterial;
  blue->diffuseColor.setValue(0,0,1);
  root->addChild(blue);

  SoDrawStyle *wireframe=new SoDrawStyle;
  wireframe->style=SoDrawStyle::LINES;
  wireframe->lineWidth=3;
  root->addChild(wireframe);

  SoLightModel *base = new SoLightModel;
  base->model=SoLightModel::BASE_COLOR;
  root->addChild(base);

  SoCube *cube = new SoCube;
  cube->height=80;
  cube->width=80;
  cube->depth=80;
  //  root->addChild(cube);  

  SoTranslation *trans = new SoTranslation();
  trans->translation.setValue(0,0,-25);
  root->addChild(trans);

  SoMaterial *red= new SoMaterial;
  red->diffuseColor.setValue(1,0,0);
  root->addChild(red);


  SoVertexProperty *property= new SoVertexProperty();
  
  const unsigned int NTIMESLICES=5000;
  const double tMax=50.0;

  for (unsigned int i=0;i<NTIMESLICES;i++) {
    double t = (tMax*i)/NTIMESLICES;
    property->vertex.set1Value(i,(*xt)(t),(*yt)(t),(*zt)(t));
  }

  SoLineSet *lines = new SoLineSet;
  lines->vertexProperty=property;
  root->addChild(lines);


  

  // Use one of the convenient SoQt viewer classes.
  SoQtExaminerViewer * eviewer = new SoQtExaminerViewer(&mainwin);
  eviewer->setTransparencyType(SoGLRenderAction::SCREEN_DOOR);
  eviewer->setSceneGraph(root);
  eviewer->setBackgroundColor(SbColor(0.6,0.6, 1));
  eviewer->show();
  
  // Pop up the main window.
  SoQt::show(&mainwin);
  // Loop until exit.
  SoQt::mainLoop();

  // Clean up resources.
  delete eviewer;
  root->unref();
  



  return 0;
}






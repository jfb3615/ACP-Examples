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
#include <QDial>
#include <QMainWindow>
#include <QToolBar>
#include <QLayout> 
#include "QatGenericFunctions/Variable.h"
#include "QatGenericFunctions/RKIntegrator.h"
#include <iostream>
#include <vector>

using namespace Genfun;
using namespace std;

QDial *xDial{nullptr};
QDial *yDial{nullptr};
QDial *zDial{nullptr};

Parameter *X0{nullptr};
Parameter *Y0{nullptr};
Parameter *Z0{nullptr};

const AbsFunction *xt{nullptr};
const AbsFunction *yt{nullptr};
const AbsFunction *zt{nullptr};
SoVertexProperty *pro{nullptr};

class SignalCatcher: public QObject {

  Q_OBJECT

  public:
  SignalCatcher (QDial *xd,QDial *yd,QDial *zd) :xd(xd),yd(yd),zd(zd){};

private:
  
  QDial *xd{nullptr};
  QDial *yd{nullptr};
  QDial *zd{nullptr};

public slots:

  void update(int) {
    X0->setValue(xd->value()/1000.0);
    Y0->setValue(yd->value()/1000.0);
    Z0->setValue(zd->value()/1000.0);

    const unsigned int NTIMESLICES=5000;
    const double tMax=50.0;

    for (unsigned int i=0;i<NTIMESLICES;i++) {
      double t = (tMax*i)/NTIMESLICES;
      pro->vertex.set1Value(i,(*xt)(t),(*yt)(t),(*zt)(t));
    }


  }

  
};

int main (int argc, char **argv) {
  #ifndef __APPLE__
  setenv("QT_QPA_PLATFORM","xcb",0);
#endif
  
  double s=10;
  double b=8.0/3.0;
  double r = 28.;
 
  Variable X(0,3),Y(1,3),Z(2,3);

  GENFUNCTION DXDT = s*(Y-X); 
  GENFUNCTION DYDT = r*X -Y -X*Z;
  GENFUNCTION DZDT = X*Y - b*Z;
  
  RKIntegrator integrator;
  
  X0=integrator.addDiffEquation(&DXDT, "X0", 2, -10, 10);
  Y0=integrator.addDiffEquation(&DYDT, "Y0", 5, -10, 10);
  Z0=integrator.addDiffEquation(&DZDT, "Z0", 5, -10, 10);
  
  xt = integrator.getFunction(X);
  yt = integrator.getFunction(Y);
  zt = integrator.getFunction(Z);
  
   //---------------------------------------------------------
  // Now that we have reboundCollection, start visualizing!

  QApplication app(argc, argv);
  QMainWindow mainwin;
  
  mainwin.setMinimumWidth(800);
  mainwin.setMinimumHeight(800);
  SoQt::init(&mainwin);


  xDial = new QDial;
  yDial = new QDial;
  zDial = new QDial;

  QToolBar *toolBar=mainwin.addToolBar("Tools");
  QAction  *quitAction=toolBar->addAction("Quit");
   
  quitAction->setShortcut(QKeySequence("q"));
  
  QObject::connect(quitAction, SIGNAL(triggered()), &app, SLOT(quit()));
  
  
  for (QDial * dial : {xDial,yDial,zDial}) {
    toolBar->addWidget(dial);
  }
  
  SignalCatcher signalCatcher(xDial,yDial,zDial);
  QObject::connect(xDial,&QDial::valueChanged,&signalCatcher,&SignalCatcher::update);
  QObject::connect(yDial,&QDial::valueChanged,&signalCatcher,&SignalCatcher::update);
  QObject::connect(zDial,&QDial::valueChanged,&signalCatcher,&SignalCatcher::update);

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


  pro= new SoVertexProperty;
  
  const unsigned int NTIMESLICES=5000;
  const double tMax=50.0;

  for (unsigned int i=0;i<NTIMESLICES;i++) {
    double t = (tMax*i)/NTIMESLICES;
    pro->vertex.set1Value(i,(*xt)(t),(*yt)(t),(*zt)(t));
  }

  SoLineSet *lines = new SoLineSet;
  lines->vertexProperty=pro;
  root->addChild(lines);


  

  // Use one of the convenient SoQt viewer classes.


  SoQtExaminerViewer * eviewer = new SoQtExaminerViewer(mainwin.centralWidget());
  eviewer->setDoubleBuffer(false);
  eviewer->setTransparencyType(SoGLRenderAction::SCREEN_DOOR);
  eviewer->setSceneGraph(root);
  eviewer->setBackgroundColor(SbColor(0.6,0.6, 1));
 
  {
    QWidget *wTmp=new QWidget;
    wTmp->setLayout(new QVBoxLayout);
    wTmp->layout()->addWidget(eviewer->getWidget());
    mainwin.setCentralWidget(wTmp);
  }

  
  
  // Pop up the main window.
  SoQt::show(&mainwin);
  // Loop until exit.
  SoQt::mainLoop();

  // Clean up resources.
  delete eviewer;
  root->unref();
  



  return 0;
}


#include "lorenz.moc"




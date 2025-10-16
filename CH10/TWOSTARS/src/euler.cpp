// HelloWorldSoQt.cpp

// SoQt includes
#include <Inventor/Qt/SoQt.h>
#include <Inventor/Qt/viewers/SoQtExaminerViewer.h>
// Coin includes
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoText3.h>
#include <Inventor/nodes/SoText2.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoTexture2.h>
#include <Inventor/nodes/SoRotationXYZ.h>
#include <Inventor/nodes/SoRotation.h>
#include <Inventor/nodes/SoScale.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/sensors/SoTimerSensor.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/SoInput.h>
#include <Inventor/SoDB.h>
#include "SoTubs.h"
#include <QApplication>
#include <QWidget>
#include <QMainWindow>
#include <QPushButton>
#include <QBoxLayout>
#include <QToolBar>
#include <QAction>
#include <iostream>
#include <libgen.h>
#include "EulerForm.h"


bool twoBodyActive{false};
bool realTime{false};

SbVec3f X(-1,0,0),Y(0,0,1),Z(0,1,0);


EulerForm *form{nullptr};
SoSwitch  *crystalSphereSwitch=nullptr;

struct State {
  SoRotation *phi={nullptr};
  SoRotation *theta{nullptr};
  SoRotation *psi{nullptr};
  SoTranslation *translation1{nullptr};
  SoTranslation *translation2{nullptr};
  SoSwitch      *planetSwitch{nullptr};
  SoSwitch      *satelliteSwitch{nullptr};
  SoSwitch      *trackSwitch{nullptr};
  int step{0};
  double timeUnit{25};
  double x{0};
  double y{0};
};







class CrystalSphereSignalCatcher : public QObject {

  Q_OBJECT

public:
  
  CrystalSphereSignalCatcher(SoSwitch *crystalSphereSwitch):flag(false),crystalSphereSwitch(crystalSphereSwitch){}
  
public slots:


  void toggle() {
    flag = !flag;
    crystalSphereSwitch->whichChild= flag ? SO_SWITCH_ALL : SO_SWITCH_NONE;
  }

private:

  bool flag;
  SoSwitch *crystalSphereSwitch;
};





class SignalCatcher: public QObject {

  Q_OBJECT
 public:
  
  SignalCatcher (State *state): pState(state){}
    
 public slots:

  void setToZero() {
  }
private:
  State *pState;
  
};



class Orbit {

public:

  Orbit():signalCatcher(&state){};
    
  State state;
  SignalCatcher signalCatcher;
  SoSeparator *get();
  double a;
  double e;
  double r{1},g{1},b{1};;


private:
  
  Orbit(const Orbit &) = delete;
  Orbit & operator= (const Orbit &) = delete;

  SoSeparator *makeOrbit(double a, double e,bool shadow=false);
  
};

SoSeparator *Orbit::get() {

  SoSeparator *root=new SoSeparator;
  
  state.phi=new SoRotation;
  state.phi->rotation.setValue(Z,0);

  state.theta=new SoRotation;
  state.theta->rotation.setValue(Y,0);

  state.psi=new SoRotation;
  state.psi->rotation.setValue(Z,0);

  state.translation1=new SoTranslation;
  state.translation2=new SoTranslation;
  state.planetSwitch = new SoSwitch;
  state.planetSwitch->whichChild= SO_SWITCH_ALL;
  state.satelliteSwitch = new SoSwitch;
  state.trackSwitch = new SoSwitch;
  state.trackSwitch->whichChild= SO_SWITCH_ALL;
  // Outer ring:
  SoSeparator *r2= new SoSeparator;
  {
    // color
    SoBaseColor *color = new SoBaseColor;
    color->rgb = SbColor(.5, .5, 1); // white
    r2->addChild(color);
    SoSeparator *r1= new SoSeparator;
    {
      // color
      SoBaseColor *color = new SoBaseColor;
      color->rgb = SbColor(r, g, b); // white
      r1->addChild(state.trackSwitch);
      state.trackSwitch->addChild(makeOrbit(a,e,true));
      r1->addChild(color);
      r1->addChild(state.phi);
      // Inner Ring:
      SoSeparator *r0=new SoSeparator;
      {

	r0->addChild(state.theta);
	{
	  SoSeparator *sep= new SoSeparator;
	  sep->addChild(state.psi);
	  sep->addChild(state.trackSwitch);
	  state.trackSwitch->addChild(makeOrbit(a,e));
	  r0->addChild(sep);
	  SoSeparator *planetSeparator = new SoSeparator;
	  SoSphere    *planetSphere    = new SoSphere;
	  SoMaterial  *planetMat       = new SoMaterial;
	  planetMat->diffuseColor.setValue(r,g,b);
	  planetSphere->radius.setValue(0.1);
	  state.planetSwitch->addChild(planetMat);
	  state.planetSwitch->addChild(state.translation1);
	  state.planetSwitch->addChild(planetSphere);
	  planetSeparator->addChild(state.planetSwitch);
	  sep->addChild(planetSeparator);


	  SoSeparator *satelliteSeparator = new SoSeparator;
	  SoSphere    *satelliteSphere    = new SoSphere;
	  SoMaterial  *satelliteMat       = new SoMaterial;
	  satelliteMat->diffuseColor.setValue(r,g,b);
	  satelliteSphere->radius.setValue(0.1);
	  state.satelliteSwitch->addChild(planetMat);
	  state.satelliteSwitch->addChild(state.translation2);
	  state.satelliteSwitch->addChild(satelliteSphere);
	  satelliteSeparator->addChild(state.satelliteSwitch);
	  sep->addChild(satelliteSeparator);


	  
	}
      }
      r1->addChild(r0);
    }
    r2->addChild(r1);
  }
  root->addChild(r2);
  return root;
}


SoSeparator *Orbit::makeOrbit(double a, double e, bool shadow) {
  SoSeparator *sep=new SoSeparator;
  SoLineSet  *lineSet= new SoLineSet;
  SoPointSet *pointSet = new SoPointSet;
  SoCoordinate3  *coords=new SoCoordinate3;

  const unsigned int NPOINTS=400;
  for (unsigned int i=0;i<NPOINTS;i++) {
    double t=i/double(NPOINTS)*2*M_PI;
    double r=a*(1+e)*(1-e)/(1+e*cos(t));
    double x=r*cos(t);
    double y=r*sin(t);
    coords->point.set1Value(i,-x,0,y);
  }
  lineSet->numVertices.setValue(shadow ? 0: NPOINTS);
  pointSet->numPoints.setValue(shadow ? NPOINTS: 0);
  SoBaseColor *color=new SoBaseColor;
  color->rgb.setValue(r,g,b);
  SoLightModel *lModel=new SoLightModel;
  lModel->model=SoLightModel::BASE_COLOR;
  SoDrawStyle *drawStyle=new SoDrawStyle;
  drawStyle->style=SoDrawStyle::LINES;
  drawStyle->lineWidth=5;
  drawStyle->pointSize=1;
  sep->addChild(color);
  sep->addChild(lModel);
  sep->addChild(drawStyle);
  sep->addChild(coords);
  sep->addChild(lineSet);
  sep->addChild(pointSet);
  return sep;
}


static void timeSensorCallback(void * vOrbit  , SoSensor * )
{
  Orbit *pOrbit=(Orbit *) vOrbit;

  
  double a=pOrbit->a;
  double e=pOrbit->e;
  static double L0{0};
  double period = pow(a,3.0/2.0);
  {

    double x0=pOrbit->state.x,y0=pOrbit->state.y;
    double theta0 = atan2(y0,x0);
    double theta  = theta0+(0.1/pOrbit->state.timeUnit/period);
  
    double r=a*(1-pow(e,2))/(1+e*cos(theta));
    double x=r*cos(theta);
    double y=r*sin(theta);
    double vx=x-x0,vy=y-y0;
    
    double Rx=(vx)*(y)-(vy)*(x);
    double L=std::fabs(Rx);
    if (L0==0.0) L0=L;

    vx*=(10.0*L0/L);
    vy*=(10.0*L0/L);

    if (realTime) {
      x=x0+vx;
      y=y0+vy;
    }

    double f=r/sqrt(x*x+y*y);
    x*=f;
    y*=f;

    
    if (!twoBodyActive) {
      pOrbit->state.translation1->translation.setValue(-x,0,y);
    }
    else {
      pOrbit->state.translation1->translation.setValue(-x/2.0,0,y/2.0);
      pOrbit->state.translation2->translation.setValue( x/2.0,0,-y/2.0);
    }
    pOrbit->state.x=x;
    pOrbit->state.y=y;
  }

}



SoSwitch *getAxes(bool lw)  {
  SoSwitch *axesSwitch = new SoSwitch();
  SoSeparator *axesSeparator = new SoSeparator;
  axesSwitch->whichChild=SO_SWITCH_ALL ;
  
  SoLightModel *lModel=new SoLightModel;
  lModel->model=SoLightModel::BASE_COLOR;
  SoDrawStyle *drawStyle=new SoDrawStyle;
  drawStyle->style=SoDrawStyle::LINES;
  drawStyle->lineWidth= lw ? 5: 1;
  axesSeparator->addChild(lModel);
  axesSeparator->addChild(drawStyle);
  
  
  std::string label[]={"X", "Y", "Z"};
  
  for (int i=0;i<3;i++) {
    SoSeparator *sep = new SoSeparator;
    SoCoordinate3 *coord = new SoCoordinate3;
    SoLineSet *lineSet=new SoLineSet;
    SoBaseColor *color=new SoBaseColor;
    coord->point.set1Value(0,SbVec3f(0,0,0));
    coord->point.set1Value(1,SbVec3f((i==0 ? -2:0), (i==2? 2: 0), (i==1 ? 2: 0)));
    color->rgb.setValue((i==0 ? 1:0), (i==1? 1:0), (i==2 ? 1:0));
    axesSeparator->addChild(sep);
    sep->addChild(color);
    sep->addChild(coord);
    sep->addChild(lineSet);
    if (lw) {
      SoTranslation *translation = new SoTranslation;
      translation->translation.setValue((i==0 ? -2.3:0), (i==2? 2.3: 0), (i==1 ? 2.3: 0));
      SoFont  *font= new SoFont;
      font->size=0.2;
      
      SoText3 *text = new SoText3;
      text->string=label[i].c_str();
      sep->addChild(translation);
      SoDrawStyle *style=new SoDrawStyle;
      sep->addChild(style);
      sep->addChild(font);
      sep->addChild(text);
    }
  }
  axesSwitch->addChild(axesSeparator);
  return axesSwitch;
}


class PlanetSignalCatcher : public QObject {

  Q_OBJECT

public:
  
  PlanetSignalCatcher(Orbit *orbit1):flag(false),orbit1(orbit1){}
  
public slots:

  void toggleRealTime() {
    realTime=!realTime;
  }
  
  void toggle() {
    flag = !flag;
    twoBodyActive=!twoBodyActive;
    orbit1->state.satelliteSwitch->whichChild=flag? SO_SWITCH_ALL: SO_SWITCH_NONE;
    orbit1->state.trackSwitch->whichChild=flag? SO_SWITCH_NONE: SO_SWITCH_ALL;

  }

private:

  bool flag;
  Orbit *orbit1;
};



int main(int argc, char ** argv)
{

#ifndef __APPLE__
  setenv ("QT_QPA_PLATFORM","xcb",0);
#endif
  QApplication app(argc, argv);

  QMainWindow mainwindow;
  //  mainwindow.resize(800, 800);
  mainwindow.setMinimumWidth(1200);
  mainwindow.setMinimumHeight(800);
  SoQt::init(&mainwindow);


  
  // Set the main node for the "scene graph" 
  SoSeparator *root = new SoSeparator;
  root->ref();

  root->addChild(getAxes(true));
  Orbit orbit;
  orbit.a=2.0/3.0;
  orbit.e=0.5;
  orbit.state.x=orbit.a*(1-orbit.e*orbit.e)/2.0;
  orbit.state.y=0;
  root->addChild(orbit.get());

  // The sun:
  {
    SoSeparator *sunSeparator= new SoSeparator;
    SoSphere *sun=new SoSphere;
    SoMaterial *mat = new SoMaterial;
    mat->emissiveColor.setValue(1,1,0);
    sun->radius.setValue(0.02);
    sunSeparator->addChild(mat);
    sunSeparator->addChild(sun);
    root->addChild(sunSeparator);
  }

  SoSwitch *crystalSphereSwitch=nullptr;
  // The Crystal Sphere:
  {
    SoSeparator *sep=new SoSeparator;
    root->addChild(sep);
    
    crystalSphereSwitch=new SoSwitch;
    crystalSphereSwitch->whichChild=SO_SWITCH_NONE;
    sep->addChild(crystalSphereSwitch);


    std::string dir=dirname(argv[0])+std::string("/../../src/");
    
    SoTexture2 *leopard = new SoTexture2;
    sep->addChild(leopard);
    leopard->filename.setValue((dir+"stars.jpg").c_str());
    leopard->model=SoTexture2::REPLACE;
    
    
    SoMaterial *mat=new SoMaterial;
    mat->diffuseColor.setValue(0,0,1);
    mat->transparency=0.4;
    crystalSphereSwitch->addChild(mat);
    crystalSphereSwitch->addChild(leopard);
    SoSphere *crystalSphere=new SoSphere;
    crystalSphere->radius=11.95;
    crystalSphereSwitch->addChild(crystalSphere);
  }
  

  // Schedule the update right here: 
  {
    SoTimerSensor * timeSensor = new SoTimerSensor;
    timeSensor->setFunction(timeSensorCallback);
    timeSensor->setData(&orbit);
    timeSensor->setBaseTime(SbTime::getTimeOfDay());
    timeSensor->setInterval(0.005f);
    timeSensor->schedule();
  }
  
  
  // Init the viewer and get a pointer to it
  SoQtExaminerViewer *ev = new SoQtExaminerViewer();
  ev->setDoubleBuffer(false);
  //  ev->setBackgroundColor(SbColor(.9,.9,1));
  ev->setDecoration(false);

  form=new EulerForm();
  
  QVBoxLayout *layout=new QVBoxLayout();
  layout->addWidget(ev->getWidget());
  layout->addWidget(form);
  
  QWidget *widget=new QWidget();
  widget->setLayout(layout);
  mainwindow.setCentralWidget(widget);
  ev->setSceneGraph(root);

  
  QToolBar *toolBar=mainwindow.addToolBar("Tools");
  QAction  *quitAction=toolBar->addAction("Quit");
  QAction  *planetAction=toolBar->addAction("2-body");
  quitAction->setShortcut(QKeySequence("q"));
  planetAction->setShortcut(QKeySequence("p"));
  QObject::connect(quitAction, SIGNAL(triggered()), &app, SLOT(quit()));

  
  

  CrystalSphereSignalCatcher catcher(crystalSphereSwitch);
  QObject::connect(form->crystalSphereButton(), &QPushButton::clicked, &catcher, &CrystalSphereSignalCatcher::toggle);
  PlanetSignalCatcher planetCatcher(&orbit);
  QObject::connect(planetAction, SIGNAL(triggered()), &planetCatcher, SLOT(toggle()));
  QObject::connect(form->realTimePushButton(), SIGNAL(clicked()), &planetCatcher, SLOT(toggleRealTime()));
  
  // Pop up the main window.
  SoQt::show(&mainwindow);
  // Loop until exit.
  SoQt::mainLoop();


  // When exiting the main loop,
  // delete the viewer and leave the handle to the main node 
  delete ev;
  root->unref();

  // exit.
  return 0;
}
#include "euler.moc"


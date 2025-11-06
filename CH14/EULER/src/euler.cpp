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
#include <Inventor/nodes/SoTexture2.h>
#include <Inventor/nodes/SoRotationXYZ.h>
#include <Inventor/nodes/SoRotation.h>
#include <Inventor/nodes/SoScale.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoMaterial.h>
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
#include "EulerForm.h"

SbVec3f X(-1,0,0),Y(0,0,1),Z(0,1,0);


EulerForm *form{nullptr};

struct State {
  SoRotation *phi={nullptr};
  SoRotation *theta{nullptr};
  SoRotation *psi{nullptr};
};


class SignalCatcher: public QObject {

  Q_OBJECT
 public:
  
  SignalCatcher (State *state): pState(state){}
    
 public slots:

  void updatePhi(double phiInt) {
    pState->phi->rotation.setValue(Z,2*M_PI*(phiInt/100.0));
  }
  void updateTheta(double thetaInt) {
    pState->theta->rotation.setValue(X,2*M_PI*(thetaInt/100.0));
  }
  void updatePsi(double psiInt) {
    pState->psi->rotation.setValue(Z,2*M_PI*(psiInt/100.0));
  }
  void setToZero() {
    form->phiDial()->setValue(0);
    form->thetaDial()->setValue(0);
    form->psiDial()->setValue(0);
  }
private:
  State *pState;
  
};

SoSeparator *getCone() {
  SoSeparator *sep = new SoSeparator();
  
  SoRotationXYZ *rot = new SoRotationXYZ;
  rot->axis=SoRotationXYZ::X;
  rot->angle=-M_PI/2;
  sep->addChild(rot);
  
  
  
  
  SoMaterial *mat=new SoMaterial();
  QColor color= "yellow";
  mat->diffuseColor.setValue(color.red()/256.0, color.green()/256.0, color.blue()/256.0);
  sep->addChild(mat);
  
  SoRotationXYZ *orient=new SoRotationXYZ();
  orient->axis=SoRotationXYZ::X;
  orient->angle=-M_PI/2;
  sep->addChild(orient);
  
  SoTranslation *trans = new SoTranslation();
  trans->translation.setValue(0,-3,0);
  sep->addChild(trans);
  
  
  SoCone *cone = new SoCone();
  cone->bottomRadius=2;
  cone->height=6;
  
  std::string dir="./";
  SoTexture2 *leopard = new SoTexture2;
  sep->addChild(leopard);
  leopard->filename.setValue((dir+"Provence104.JPG").c_str());
  leopard->model=SoTexture2::REPLACE;
  
  sep->addChild(cone);
  return sep;
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
    coord->point.set1Value(1,SbVec3f((i==0 ? -7:0), (i==2? 7: 0), (i==1 ? 7: 0)));
    color->rgb.setValue((i==0 ? 1:0), (i==1? 1:0), (i==2 ? 1:0));
    axesSeparator->addChild(sep);
    sep->addChild(color);
    sep->addChild(coord);
    sep->addChild(lineSet);
    if (!lw) {
      SoTranslation *translation = new SoTranslation;
      translation->translation.setValue((i==0 ? -8:0), (i==2? 8: 0), (i==1 ? 8: 0));
      SoFont  *font= new SoFont;
      font->size=2;
      
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




int main(int argc, char ** argv)
{

#ifndef __APPLE__
  setenv ("QT_QPA_PLATFORM","xcb",0);
#endif  
  const double ringScale=5.0;
  QApplication app(argc, argv);

  QMainWindow mainwindow;
  //  mainwindow.resize(800, 800);
  mainwindow.setMinimumWidth(800);
  mainwindow.setMinimumHeight(800);
  SoQt::init(&mainwindow);
  SoTubs::initClass();


  State state;
  
  // Set the main node for the "scene graph" 
  SoSeparator *root = new SoSeparator;
  root->ref();

  root->addChild(getAxes(false));

  state.phi=new SoRotation;
  state.phi->rotation.setValue(Z,0);

  state.theta=new SoRotation;
  state.theta->rotation.setValue(Y,0);

  state.psi=new SoRotation;
  state.psi->rotation.setValue(Z,0);


  // Outer ring:
  SoSeparator *r2= new SoSeparator;
  {
    // color
    SoBaseColor *color = new SoBaseColor;
    color->rgb = SbColor(.5, .5, 1); // white
    r2->addChild(color);
    SoTubs *c2=new SoTubs;
    c2->pRMin=1.1*ringScale;
    c2->pRMax=1.2*ringScale;
    c2->pDz=0.02  *ringScale;
    r2->addChild(c2);
    // Middle ring:
    SoSeparator *r1= new SoSeparator;
    {
      // color
      SoBaseColor *color = new SoBaseColor;
      color->rgb = SbColor(1, 1, 1); // white
      r1->addChild(color);
      SoTubs *c1=new SoTubs;
      c1->pRMin=1.0*ringScale;
      c1->pRMax=1.1*ringScale;
      c1->pDz=0.02*ringScale;
      r1->addChild(state.phi);
      r1->addChild(c1);
      // Inner Ring:
      SoSeparator *r0=new SoSeparator;
      {
	// color
	SoBaseColor *color = new SoBaseColor;
	color->rgb = SbColor(1, 0, 0); // Red
	r0->addChild(color);
	SoSeparator *c0= new SoSeparator;
	{
	  SoTubs *c00=new SoTubs;
	  c00->pRMin=0.9*ringScale;
	  c00->pRMax=1.0*ringScale;
	  c00->pDz=0.02*ringScale;
	  //SoRotation *pR=new SoRotation;
	  //pR->rotation.setValue(Z,M_PI/2.0);
	  //c0->addChild(pR);
	  c0->addChild(c00);
	}
	r0->addChild(state.theta);
	r0->addChild(c0);
	{
	  SoSeparator *sep= new SoSeparator;
	  sep->addChild(state.psi);
	  sep->addChild(getAxes(true));
	  if (argc==2) {
	    SoInput input;
	    if (!input.openFile(argv[1])) {
	      std::cerr << "Cannot open inventor file " << argv[1] << std::endl;
	      return 1;
	    }
	    else {
	      SoSeparator *myInput=SoDB::readAll(&input);
	      if (myInput==nullptr) {
		std::cerr << "Problem reading inventor file " << argv[1] << std::endl;
		return 1;
	      }
	      else {
		{
		  SoScale *scale = new SoScale;
		  scale->scaleFactor.setValue(0.8,0.8, 0.8);
		  SoTranslation *translation=new SoTranslation;
		  translation->translation.setValue(0,-1,1);

		  sep->addChild(scale);
		  sep->addChild(translation);
		  sep->addChild(myInput);
		  input.closeFile();
		}
	      }
	    }
	  }
	  else {
	    sep->addChild(getCone());
	  }
	  r0->addChild(sep);
	}
      }
      r1->addChild(r0);
    }
    r2->addChild(r1);
  }
  root->addChild(r2);


  
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
  quitAction->setShortcut(QKeySequence("q"));
  QObject::connect(quitAction, SIGNAL(triggered()), &app, SLOT(quit()));

  SignalCatcher signalCatcher(&state);
  QObject::connect(form->phiDial(),     &QDial::valueChanged, &signalCatcher, &SignalCatcher::updatePhi);
  QObject::connect(form->thetaDial(),   &QDial::valueChanged, &signalCatcher, &SignalCatcher::updateTheta);
  QObject::connect(form->psiDial(),     &QDial::valueChanged, &signalCatcher, &SignalCatcher::updatePsi);
  QObject::connect(form->resetButton(), &QPushButton::clicked,&signalCatcher, &SignalCatcher::setToZero);
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


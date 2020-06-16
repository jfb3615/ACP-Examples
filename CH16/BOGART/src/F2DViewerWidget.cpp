#include "F2DViewerWidget.h"
#include "QatGenericFunctions/Argument.h"
#include "QatGenericFunctions/AbsFunction.h"

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/Qt/viewers/SoQtExaminerViewer.h>
#include <Inventor/Qt/viewers/SoQtPlaneViewer.h>
#include <Inventor/nodes/SoCamera.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/nodes/SoMaterialBinding.h>
#include <Inventor/nodes/SoNormal.h>
#include <Inventor/nodes/SoIndexedPointSet.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoDrawStyle.h>

#include <QBoxLayout>

class F2DViewerWidget::Clockwork {
public:
  SoSeparator *root=NULL;
  unsigned int NX;
  unsigned int NY;
  double       XMIN;
  double       YMIN;
  double       XMAX;
  double       YMAX;
  SoCoordinate3 *coordinates=NULL;
  SoPointSet    *pointSet   =NULL;
  SoMaterial    *faceMaterial=NULL;
  const Genfun::AbsFunction *function=NULL;
  void init(F2DViewerWidget *This);
};

void F2DViewerWidget::addPoint(double x, double y, double z) {
  if (!c->root) capture();
  unsigned int numPoints=c->pointSet->numPoints.getValue();
  c->pointSet->numPoints=numPoints+1;
  c->coordinates->point.set1Value(numPoints,x,y,z);
}

void F2DViewerWidget::setFunction (const Genfun::AbsFunction * F) {
  if (!c->root) capture();
  c->function=F;
  unsigned int index=0;
  double x0= c->XMIN, x1=c->XMAX,     dx=(x1-x0)/double(c->NX);;
  double y0= c->YMIN, y1=c->YMAX,     dy=(y1-y0)/double(c->NY);;
  for (unsigned int i=0;i<c->NX+1;i++) {
    double x = x0+i*dx;
    for (unsigned int j=0;j<c->NY;j++) {
      double y = y0+j*dy;
      Genfun::Argument arg(2); arg[0]=x; arg[1]=y;

      // We multiply by an area since we determine a density...
      double intensity=c->function ? (*c->function)(arg):1.0;
     

      intensity=std::min(1.0, intensity);
      intensity=std::max(0.0, intensity);
      c->faceMaterial->diffuseColor.set1Value(index,intensity,intensity,intensity);

      index++;
    }
  }
}


F2DViewerWidget::F2DViewerWidget(QWidget *parent, 
				 unsigned int NX, double XMIN, double XMAX, 
				 unsigned int NY, double YMIN, double YMAX): QWidget(parent),c(new Clockwork) {
  
  c->NX=NX;
  c->NY=NY;
  c->XMIN=XMIN;
  c->YMIN=YMIN;
  c->XMAX=XMAX;
  c->YMAX=YMAX;

}

void F2DViewerWidget::setSize(unsigned int NX, double XMIN, double XMAX, 
			      unsigned int NY, double YMIN, double YMAX) {
  c->NX=NX;
  c->NY=NY;
  c->XMIN=XMIN;
  c->YMIN=YMIN;
  c->XMAX=XMAX;
  c->YMAX=YMAX;
  
}

F2DViewerWidget::~F2DViewerWidget() {
  c->root->unref();
  delete c;
}

void F2DViewerWidget::capture() {
  if (c->root) throw std::runtime_error("F2DViewer::capture called multiple times, should only happen once");
  c->init(this);
}

void F2DViewerWidget::Clockwork::init(F2DViewerWidget *This) {
  root = new SoSeparator;
  root->ref();
  coordinates = new SoCoordinate3();
  pointSet =new SoPointSet;
  pointSet->numPoints=0;
  SoDrawStyle   *drawStyle=new SoDrawStyle;
  drawStyle->pointSize=1;


  SoCoordinate3  *mainCoordinates = new SoCoordinate3;
  SoIndexedShape *faceSet=new SoIndexedFaceSet;
  SoMaterialBinding *binding = new SoMaterialBinding;
  SoNormal *mainNormals = new SoNormal;
  faceMaterial=new SoMaterial;

  //  unsigned int NP=50;
  SoLightModel *model=new SoLightModel;
  model->model=SoLightModel::PHONG;

  binding->value=SoMaterialBinding::PER_VERTEX_INDEXED;
  double x0= XMIN, x1=XMAX,     dx=(x1-x0)/double(NX);;
  double y0= YMIN, y1=YMAX,     dy=(y1-y0)/double(NY);;
  unsigned int index=0;
  double iMin=HUGE_VAL, iMax=-HUGE_VAL;
  for (unsigned int i=0;i<NX+1;i++) {
    double x = x0+i*dx;
    for (unsigned int j=0;j<NY;j++) {
      double y = y0+j*dy;


      mainCoordinates->point.set1Value(index, x,y,0);
      mainNormals->vector.set1Value(index,0,0,1);


      Genfun::Argument arg(2); arg[0]=x; arg[1]=y;

      // We multiply by an area since we determine a density...
      double intensity=function ? (*function)(arg):1.0;
     
      iMin=std::min(iMin,intensity);
      iMax=std::max(iMax,intensity);

      intensity=std::min(1.0, intensity);
      intensity=std::max(0.0, intensity);
      faceMaterial->diffuseColor.set1Value(index,intensity,intensity,intensity);

      index++;
    }
  }
  //
  // Now index
  //
  unsigned int counter=0;
  for (unsigned int i=0;i<NX;i++) {
    for (unsigned int j=0;j<NY;j++) {
      
      unsigned int base0 = NY*i+j;
      unsigned int base1 = NY*(i+1)+j;
      unsigned int base2 = j+1==NY ? 0 : base0+1;
      unsigned int base3 = j+1==NY ? 0 : base1+1;
      
      if (base2!=0) {
	faceSet->materialIndex.set1Value(counter,base0);
	faceSet->normalIndex.set1Value(counter,base0);
	faceSet->coordIndex.set1Value(counter ,base0);
	counter++;
        
        
	faceSet->materialIndex.set1Value(counter,base1);
	faceSet->normalIndex.set1Value(counter,base1);
	faceSet->coordIndex.set1Value(counter ,base1);
	counter++;
        
        
	faceSet->materialIndex.set1Value(counter,base2); 
	faceSet->normalIndex.set1Value(counter,base2); 
	faceSet->coordIndex.set1Value(counter ,base2); 
	counter++;
        
        
	faceSet->materialIndex.set1Value(counter,-1);
	faceSet->normalIndex.set1Value(counter,-1);
	faceSet->coordIndex.set1Value(counter ,-1);
	counter++;
        
        
	faceSet->materialIndex.set1Value(counter,base1);
	faceSet->normalIndex.set1Value(counter,base1);
	faceSet->coordIndex.set1Value(counter ,base1);
	counter++;
        
        
	faceSet->materialIndex.set1Value(counter,base3);
	faceSet->normalIndex.set1Value(counter,base3);
	faceSet->coordIndex.set1Value(counter ,base3);
	counter++;
        
        
	faceSet->materialIndex.set1Value(counter,base2); 
	faceSet->normalIndex.set1Value(counter,base2); 
	faceSet->coordIndex.set1Value(counter ,base2); 
	counter++;
        
        
	faceSet->materialIndex.set1Value(counter,-1);
	faceSet->normalIndex.set1Value(counter,-1);
	faceSet->coordIndex.set1Value(counter ,-1);
	counter++;
      }          
      
    }
  }
  
  faceSet->materialIndex.set1Value(counter,-2);
  faceSet->normalIndex.set1Value(counter,-2);
  faceSet->coordIndex.set1Value(counter ,-2);
  counter++;
  
  SoLightModel *m2=new SoLightModel;
  m2->model=SoLightModel::BASE_COLOR;

  if (1) {
    SoMaterial *mt2=new SoMaterial;
    mt2->diffuseColor.setValue(0,0,1);
    root->addChild(mt2);
    root->addChild(m2);
    root->addChild(drawStyle);
    root->addChild(coordinates);
    root->addChild(pointSet);
  }

  root->addChild(model);
  root->addChild(mainCoordinates);
  root->addChild(mainNormals);
  root->addChild(faceMaterial);
  root->addChild(binding);
  root->addChild(faceSet);


  //
  // Make an examiner viewer:
  //
  SoQtPlaneViewer * eviewer = new SoQtPlaneViewer();
  QVBoxLayout *layout=new QVBoxLayout();
  layout->addWidget(eviewer->getWidget());
  This->setLayout(layout);
  eviewer->setSceneGraph(root);
  eviewer->viewAll();

}

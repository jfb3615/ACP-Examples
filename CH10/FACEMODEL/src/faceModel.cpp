// HelloWorldSoQt.cpp

// SoQt includes
#include <Inventor/Qt/SoQt.h>
#include <Inventor/Qt/viewers/SoQtExaminerViewer.h>
// Coin includes
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoLightModel.h>
#include <QApplication>
#include <QWidget>
#include <numbers>
int main(int argc, char ** argv)
{
  
#ifndef __APPLE__
  setenv ("QT_QPA_PLATFORM","xcb",0);
#endif
  
  QApplication app(argc, argv);

  QWidget mainwindow;
  mainwindow.resize(400, 400);

  SoQt::init(&mainwindow);
  
  // Set the main node for the "scene graph" 
  SoSeparator *root = new SoSeparator;
  root->ref();

  SoCoordinate3 *coords=new SoCoordinate3;
  constexpr double degrees=std::numbers::pi/180.0;
  constexpr double R=10.0;
  unsigned int c{0},k{0};

  double face[12][31]{
    {6, 6, 6, 6, 4, 4, 4,3.6, 3.3, 1.7, 1.8, 2.3, 1.9, 1.7,1.9, 0, 0, 0, 0, 0.2, 1.4, 1.5, 1.5, 0.9, 0.9, 0.9, 1.0, 1.2, 1.5, 2.3, 10.0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};


 

  SoIndexedFaceSet *faceSet=new SoIndexedFaceSet;
  
  
  for (int zIndex=-15;zIndex<=15;zIndex++) {
    for (int phiIndex=0;phiIndex<=12; phiIndex++) {
      double phi=30*phiIndex*degrees;
      double RPrime=R-face[phiIndex%12][zIndex+15];
      double x=RPrime*std::cos(phi);
      double y=RPrime*std::sin(phi);
      double z=zIndex;
      coords->point.set1Value(c, x,z,y);
      // Indexing is a little tricky.  This sets the order of the vertices
      // so that surface normals to each face points outward.  The direction
      // of the normal determines how light is reflected from each face.
      if (zIndex<15 && phiIndex<12) {
	faceSet->coordIndex.set1Value(k++,c);
	faceSet->coordIndex.set1Value(k++,c+13);
	faceSet->coordIndex.set1Value(k++,c+14);
	faceSet->coordIndex.set1Value(k++,c+1);
	faceSet->coordIndex.set1Value(k++,SO_END_FACE_INDEX);
      }
      c++;
    }
  }

  SoDrawStyle *drawStyle=new SoDrawStyle;
  drawStyle->style=SoDrawStyle::LINES;

  SoLightModel *lightModel=new SoLightModel;
  lightModel->model=SoLightModel::BASE_COLOR;

  
  //root->addChild(lightModel);
  //root->addChild(drawStyle);
  root->addChild(coords);
  root->addChild(faceSet);
  
  // Init the viewer and get a pointer to it
  SoQtExaminerViewer *ev = new SoQtExaminerViewer(&mainwindow);
  ev->setDoubleBuffer(false);
  
  // Set the main node as content of the window and show it
  ev->setSceneGraph(root);
  ev->show();
  
  // Start the windowing system and show our window 
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

//
// world.cpp
//
// by Riccardo Maria BIANCHI  Jun. 2015
//
// based on code by Stephan Siemen
//

// Coin3D includes
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTexture2.h>
#include <Inventor/nodes/SoSphere.h>
// SoQt includes
#include <Inventor/Qt/SoQt.h>
#include <Inventor/Qt/viewers/SoQtExaminerViewer.h>
#include <QWidget>
#include <QApplication>

  
int main(int argc, char ** argv)
{

  QApplication app(argc, argv);
  
  
  QWidget window;
  window.resize(400, 400);

  SoQt::init(&window);
  
  
  // init the main node of the "scene graph"
  SoSeparator *root = new SoSeparator;
  root->ref();

  // A node for the whole Earth object
  SoSeparator *earth = new SoSeparator;
  
  // A node for the texture
  SoTexture2 *texture_earth = new SoTexture2;
  
  // Name of texture file
  texture_earth->filename = "world32k.jpg";
  
  // Add texture to group node
  earth->addChild(texture_earth);
  
  // Add sphere to group node
  // and draws the texture image
  // on the surface of the sphere shape
  earth->addChild(new SoSphere);
  
  // Add the Earth node to the root node
  root->addChild(earth);
  
  
  // set the view with the root node and show it
  SoQtExaminerViewer *b = new SoQtExaminerViewer(&window);
  b->setTitle("Textured Earth");
  b->setSceneGraph(root);
  b->show();
  
  // Start the Qt window and show it
  SoQt::show(&window);
  
  // Loop until exit.
  SoQt::mainLoop();

  // When exit from the main loop,
  // delete the SoQtExaminerViewer hosting the scenegraph...
  delete b;
  // ...and remove the handle to the main root node
  root->unref();

  // then, exit.
  return 0;
}

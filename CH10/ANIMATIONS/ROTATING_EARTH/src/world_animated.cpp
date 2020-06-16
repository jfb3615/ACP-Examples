//
// world_animated.cpp
//
// by Riccardo Maria BIANCHI  Jun. 2015
//
// based on code by Stephan Siemen
//

// Coin nodes
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTexture2.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoRotationXYZ.h>
// Coin engines
#include <Inventor/engines/SoCalculator.h>
#include <Inventor/engines/SoTimeCounter.h>
// SoQt includes
#include <Inventor/Qt/SoQt.h>
#include <Inventor/Qt/viewers/SoQtExaminerViewer.h>
#include <QApplication>
#include <QWidget>


SoSeparator* drawEarth()
{
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
  
  return earth;
}



int main(int argc, char ** argv)
{

  // Initialize the application:
  QApplication app(argc, argv);

  // Make a main window;
  QWidget window;
  window.resize(400,400);
  
  // Init the SoQt library
  SoQt::init(&window);
  
  // init the main node of the "scene graph"
  SoSeparator *root = new SoSeparator;
  root->ref();
  
  // A separator for the Earth
  SoSeparator *earth = new SoSeparator;
  
  // Create a rotation node
  // and add it to the local 'earth' separator
  // *before* adding the Earth shape object
  SoRotationXYZ *earthRotation = new SoRotationXYZ;
  earthRotation->axis.setValue("Y"); // set the rotation axis
  earth->addChild(earthRotation);
  
  // Create the Earth object
  // and add it to the local 'earth' separator
  earth->addChild( drawEarth() );
  
  // Create a 'TimeCounter' engine node
  // It is an integer counter, with one output
  SoTimeCounter *counterEngine = new SoTimeCounter;
  counterEngine->max=360;  // set max steps
  counterEngine->step=1;   // set step value
  counterEngine->frequency=0.03; // set the frequency (# of complete cycles min->max per second)
  
  // Create a converter using a 'Calculator' engine node
  // SoCalculator has 8 scalar inputs [a,...,h]
  // and 4 scalar outputs [oa, ..., od]
  // We convert Degrees -> Radians on-the-fly
  // using the input 'a' and the output 'oa'
  SoCalculator *converterDegRad = new SoCalculator;
  converterDegRad->a.connectFrom( &counterEngine->output ); // connect 'output' of the counter engine to 'a'
  converterDegRad->expression.set1Value(0,"oa=a/(2*M_PI)"); // set the (Deg->Rad) conversion formula
  
  // Connect the converter output 'oa'
  // to the 'angle' attribute Earth rotation node
  earthRotation->angle.connectFrom( &converterDegRad->oa );
  
  // Add the Earth node to the root node
  root->addChild( earth );
  
  
  // set the view with the root node and show it
  SoQtExaminerViewer *b = new SoQtExaminerViewer(&window);
  b->setTitle("Rotating Earth");
  b->setSceneGraph(root);
  b->show();
  
  // Start the Qt window and show it
  SoQt::show(&window);
  
  // Loop until exit.
  SoQt::mainLoop();
  
  // When exit from the main loop,
  // delete the SoQtExaminerViewer hosting the scene graph...
  delete b;
  // ...and remove the handle to the main root node
  root->unref();
  
  // then, exit.
  return 0;
}

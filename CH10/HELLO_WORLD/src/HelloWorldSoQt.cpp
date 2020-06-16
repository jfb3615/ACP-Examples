// HelloWorldSoQt.cpp

// SoQt includes
#include <Inventor/Qt/SoQt.h>
#include <Inventor/Qt/viewers/SoQtExaminerViewer.h>
// Coin includes
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoText3.h>
#include <Inventor/nodes/SoSeparator.h>
#include <QApplication>
#include <QWidget>
int main(int argc, char ** argv)
{
  QApplication app(argc, argv);

  QWidget mainwindow;
  mainwindow.resize(400, 400);

  SoQt::init(&mainwindow);
  
  // Set the main node for the "scene graph" 
  SoSeparator *root = new SoSeparator;
  root->ref();

  // Set the color for the text (in RGB mode)
  SoBaseColor *color = new SoBaseColor;
  color->rgb = SbColor(1, 1, 0); // Yellow
  root->addChild(color);

  // Set the node for the 3D text, set the visible surfaces 
  // and add actual text to it
  SoText3 * text3D = new SoText3();
  text3D->string.setValue("Hello World!");
  text3D->parts.setValue("ALL");
  root->addChild(text3D);
  
  // Init the viewer and get a pointer to it
  SoQtExaminerViewer *ev = new SoQtExaminerViewer(&mainwindow);

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

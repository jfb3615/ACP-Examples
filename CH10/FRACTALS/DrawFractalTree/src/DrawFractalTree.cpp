//
// DrawFractalTree.cpp
//

// SoQt includes
#include <Inventor/Qt/SoQt.h>
#include <Inventor/Qt/viewers/SoQtExaminerViewer.h>
// Coin includes
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoGroup.h>
#include<Inventor/nodes/SoLineSet.h>
#include<Inventor/nodes/SoDrawStyle.h>
#include<Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <QApplication>
#include <QWidget>
// C++ includes
#include <iostream>
#include <cmath>
#include <algorithm>


//___________________________________________________________________
void drawLine(float x1, float y1, float x2, float y2, SoGroup* group)
{
    // Declare a new Separator to contain the line    
    SoSeparator *sep = new SoSeparator;
    
    // Declaring a set of 3D coordinates.
    SoCoordinate3* points = new SoCoordinate3;

    //Using the two points as coordinates
    points->point.set1Value(0, x1, y1, 0); 
    points->point.set1Value(1, x2, y2, 0);

    // Declare a new LineSet, to draw our line    
    SoLineSet *pLineSet = new SoLineSet;
  
    // Build the line object, adding to the Separator node... 
    // ...first, the two points representing 
    // the start and the end of the line...
    sep->addChild(points);
    // ...then, the line, which will be
    // drawn between the two points
    sep->addChild(pLineSet);
    
    // Add the line to the scene graph
    group->addChild(sep);
}


//___________________________________________________________________________________
void drawTree(float x, float y, float len, float angle, float stroke, SoGroup* group)
{        
  if (len >= 1.) {  // Stop the recursion when length of line is < 1.0
      
      // First, set up the line style...
      SoDrawStyle* style = new SoDrawStyle();
      style->lineWidth = std::max(0.5,stroke/2.0);
      group->addChild(style);
      
      // Draw the "mother branch" line in the current separator
      drawLine(x, y, x, y+len, group);
      
      // Then, create two new separators, 
      // to host the transformations and the "children branches"...
      SoSeparator* trSep1 = new SoSeparator;
      SoSeparator* trSep2 = new SoSeparator;
      // ... and add them to the "mother" Separator
      group->addChild(trSep1);
      group->addChild(trSep2);
    
      // Declare a new transformation, one for each "child branch"
      SoTransform* myTransform1 = new SoTransform;
      SoTransform* myTransform2 = new SoTransform;
     
      // Set rotation center, axis, and angle
      myTransform1->center.setValue(SbVec3f(0,y+len,0));       
      myTransform1->rotation.setValue(SbVec3f(0,0,1), angle); 
      myTransform2->center.setValue(SbVec3f(0,y+len,0));       
      myTransform2->rotation.setValue(SbVec3f(0,0,1), -angle);
      
      // add the transformations to the "child" Separator
      trSep1->addChild(myTransform1);
      trSep2->addChild(myTransform2);
      
      // length of sub branches = 2/3 of the main branch
      float branch_len = len * 0.66;
      // set the stroke width of the sub branch
      stroke = stroke * 0.66;
      
      // Recursive calls generate each sub-branch:
      drawTree(x, y+len, branch_len, angle, stroke, trSep1);
      drawTree(x, y+len, branch_len, -angle, stroke, trSep2);
  } 
  
}

//______________________________
int main(int argc, char **argv)
{

  QApplication app(argc, argv);
  // Init the Qt windowing system
  // and get a pointer to the window 

  QWidget window;
  SoQt::init(&window);
  
  // Set the main node for the "scene graph" 
  SoSeparator *root = new SoSeparator;
  root->ref();

  // Declare a Group node to store the tree.
  SoGroup* group = new SoGroup();
  // Call the method which draw the tree
  // (we pass the Group node)
  drawTree(0, 0, 200, M_PI/6., 10., group);

  // Add to the scene graph the Group node containing the tree
  root->addChild(group);
  
  // Init the viewer and get a pointer to it
  SoQtExaminerViewer *b = new SoQtExaminerViewer(&window);

  // Set the main node as content of the window and show it
  b->setSceneGraph(root);
  b->show();
  
  
  // Start the windowing system and show our window 
  SoQt::show(&window);
  // Loop until exit.
  SoQt::mainLoop();

  // When exiting the main loop,
  // delete the viewer and leave the handle to the main node 
  delete b;
  root->unref();

  // exit.
  return 0;
}



// DrawCircle_1.cpp
//
// by Riccardo Maria BIANCHI, Jun 2015
//

// SoQt includes
#include <Inventor/Qt/SoQt.h>
#include <Inventor/Qt/viewers/SoQtExaminerViewer.h>
#include <Inventor/Qt/viewers/SoQtFlyViewer.h>
#include <Inventor/Qt/viewers/SoQtPlaneViewer.h>

// Coin includes
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoTransparencyType.h>
#include <Inventor/SoRenderManager.h>

#include<Inventor/nodes/SoLineSet.h>
#include<Inventor/nodes/SoDrawStyle.h>
#include<Inventor/nodes/SoCoordinate3.h>

#include <Inventor/nodes/SoRotation.h>
#include <Inventor/nodes/SoRotationXYZ.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoTransform.h>
#include <QApplication>
#include <QWidget>

#include <iostream>

#include <random> // C++11
  

#define _USE_MATH_DEFINES
#include <math.h> // together with _USE_MATH_DEFINES, it defines M_PI




// Setup a random numbers generator for branches angle, with the random seed, to have different tree shape each time
std::random_device rd; // take a random seed from the operating system
std::mt19937 mt(rd()); // init the random numbers with the random seed
std::uniform_real_distribution<double> distAngle_u(0, M_PI/3.); // set the range for the random numbers, in this case a uniform distribution


/* Map a float value to range [0., 1.] */
float mapValueRangeZeroOne(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}





void drawLine(float x1, float y1, float x2, float y2, float z1, float z2, SoGroup* group)
{
    //std::cout << "\n - drawLine - x1,y1: " << x1 << "," << y1 << " - x2,y2: " << x2 << "," << y2 << std::endl;
    
    SoSeparator *sep = new SoSeparator;
    
    // Declaring a set of 3D coordinates.
    // The SoCoordinate3 seems to contain an array of vectors representing
    // the points in the polyline, so you add them by calling Set1Value on the
    // point attribute, passing in the index of the point and the vector each time.
    SoCoordinate3* points = new SoCoordinate3;

    // Setting two points: the start and the end of the line
    const SbVec3f p1 =  SbVec3f(x1, y1, z1);
    const SbVec3f p2 =  SbVec3f(x2, y2, z2);

    //Using the two points as coordinates
    points->point.set1Value(0, p1); 
    points->point.set1Value(1, p2);
            

    //static int numPoints[] = {2}; // this tells to draw a line between the first and second 3D-point
    
    SoLineSet *pLineSet = new SoLineSet;
  
    sep->addChild(points);
    sep->addChild(pLineSet);
    
    group->addChild(sep);
}

void drawTree(float x, float y, float z, float len, float angle, float stroke, SoGroup* group)
{        
  if (len >= 1.) {  // stop the recursion when length of line is < 1
    
      //std::cout << "len, angle: " << len << ", " << angle << std::endl;                     
     
      // First, set up the line style...
      SoDrawStyle* style = new SoDrawStyle();
      style->lineWidth = std::max(0.5,stroke/2.);
      group->addChild(style);
      
      // ...and the material/color
      SoMaterial* sm = new SoMaterial();
      /* Map the len value [lenMin, lenMax] to 8 bits (0 to 255) */
      float colorValue = mapValueRangeZeroOne(len, 0., 200., 0., 1.);
      sm->diffuseColor.setValue(0.002, colorValue+0.1, colorValue+0.1);     
      group->addChild(sm);
     
      // Then, draw the "mother branch" line in the current separator
      // the line will be drawn with current line style and material, 
      // defined above
      drawLine(x, y, x, y, z, z+len, group);                 
     
      // then create two new separators, to host the transformations and the "children branches"
      SoSeparator* trSep1 = new SoSeparator;
      SoSeparator* trSep2 = new SoSeparator;

      group->addChild(trSep1);
      group->addChild(trSep2);
    
      // declare two new transformations, for the two 'branches'
     SoTransform* myTransform1 = new SoTransform;
     SoTransform* myTransform2 = new SoTransform;
          
          
     // set a fixed value for the branch angles...
     //float theta1 = theta2 = angle;
     // ... OR set a random angle for the children branches
     float theta1 = distAngle_u(mt);
     float theta2 = distAngle_u(mt);
     
     
     // set the values for center of rotation and rotation angle and axis
     // NOTE: the transformations are applied in the order: 1. set center, 2. apply rotation
     myTransform1->rotation.setValue(SbVec3f(0,1,0), theta1);  // 2. rotate of 'theta1' around the new Y axis
     myTransform1->center.setValue(SbVec3f(x,y,z+len));       // 1. set the center of rotation at (0,len,0), i.e. the end of the mother line
     myTransform2->rotation.setValue(SbVec3f(0,1,0), -theta2); // 2. rotate of '-theta2' around the new Y axis
     myTransform2->center.setValue(SbVec3f(x,y,z+len));       // 1. set the center of rotation at (0,len,0), i.e. the end of the mother line          
     
     
     // add the 'branch' transformations to the 'branch' separators
     trSep1->addChild(myTransform1);
     trSep2->addChild(myTransform2);          
     
     
     // we set the length for children branch
     float branch_len = len * 0.66;
     // set the stroke width of child branch
     stroke = stroke * 0.66;

    // call recursively passing the current separator, to draw the first "branch" (which is the first 1/3 of the mother branch)
    drawTree(x, y, z+len, branch_len, angle, stroke, trSep1);
    // draw the second "branch"
    drawTree(x, y, z+len, branch_len, angle, stroke, trSep2);
  } 
    
}

void makeRandomForest (int n_trees, SoGroup* group) 
{
    // tree settings
    float len = 200.;      // the max length of the tree trunk
    float stroke =  10.;   // thickness of the trunk
    float angle = M_PI/6.; // the min branches angle
    
    // forest settings
    float side_size = 1000.; // the side length of the forest
    
    // init random numbers. 
    // See:
    // - http://stackoverflow.com/a/19666713/320369
    // - http://en.cppreference.com/w/cpp/numeric/random
    // THIS
    //std::random_device rd; // take a random seed from the operating system
    //std::mt19937 mt(rd()); // init the random numbers with the random seed
    // OR
    std::mt19937 mt(20.); // init the random numbers with a fixed seed, for reproduceable results
    std::uniform_real_distribution<double> dist_u(0, side_size); // set the range for the random numbers, in this case a uniform distribution
    
    std::normal_distribution<double> dist_n(0, side_size); // set the range for the random numbers, in this case a uniform distribution
    // for length
    std::normal_distribution<double> distLen_n(1, len+1.); // set the range for the random numbers, in this case a uniform distribution
    // for angle
    std::normal_distribution<double> distAng_n(angle, M_PI/4.); // set the range for the random numbers, in this case a uniform distribution
  
    for (int i=0; i < n_trees; ++i) 
    {   
        // get random (x,y) coordinate for the new tree
        float x = dist_u(mt);
        float y = dist_u(mt);
        float z = 0.; // fixed Z = 0
        // get random values for the size of the trunk 
        len = distLen_n(mt);
        //angle = distAng_n(mt); // random angle of the branches
        std::cout << "new tree at: (" << x << ", " << y << ", " << z << ")" << std::endl;
        SbVec3f position(x, y, z);                   
  
        drawTree(x, y, 0, len, angle, stroke, group);
    }
}

int main(int argc, char **argv)
{

  // Start the application:
  QApplication app(argc, argv);

  // Make a window and set its size
  QWidget window;
  window.resize(400, 400);
  
  // Init the Qt windowing system
  SoQt::init(&window);
	     
  // Set the main node for the "scene graph" 
  SoSeparator *root = new SoSeparator;
  root->ref();

  // tree settings
  int n_trees = 10;
  
  SoGroup* group = new SoGroup();
  
  // create a random forest
  makeRandomForest( n_trees, group );
  

  //root->addChild(matSphere);  
  root->addChild(group);
  
  // Init the viewer and get a pointer to it
  SoQtExaminerViewer *b = new SoQtExaminerViewer(&window);
  //SoQtFlyViewer *b = new SoQtFlyViewer(window);
  //SoQtPlaneViewer *b = new SoQtPlaneViewer(window);

  // Set the main node as content of the window and show it
  b->setSceneGraph(root);
  b->show();
  
  // set draw and transparency type
  b->setTransparencyType(SoGLRenderAction::SORTED_OBJECT_BLEND);
  //b->setDrawStyle(SoQtViewer::STILL, SoQtViewer::VIEW_WIREFRAME_OVERLAY); // WIREFRAME on top of normal view
  b->setDrawStyle(SoQtViewer::STILL, SoQtViewer::VIEW_LINE); // WIREFRAME
  
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



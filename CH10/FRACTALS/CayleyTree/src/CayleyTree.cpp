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
#include <sstream>
#include <cmath>


//___________________________________________________________________
void drawLine(float x1, float y1, float x2, float y2, SoGroup* group)
{
    // Declare a new Separator to contain the line    
    SoSeparator *sep = new SoSeparator;
    
    // Declaring a set of 3D coordinates.
    // The SoCoordinate3 seems to contain an array of vectors representing
    // the points in the polyline, so you add them by calling Set1Value on the
    // point attribute, passing in the index of the point and the vector each time.
    SoCoordinate3* points = new SoCoordinate3;

    // Setting two points: the start and the end of the line
    const SbVec3f p1 =  SbVec3f(x1, y1, 0);
    const SbVec3f p2 =  SbVec3f(x2, y2, 0);

    //Using the two points as coordinates
    points->point.set1Value(0, p1); 
    points->point.set1Value(1, p2);

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
void drawTree(float x, float y, float len, int coordNumber, int nGenMax, float angle, float stroke, SoGroup* group)
{           
    if (nGenMax > 0) {  // Stop the recursion if all generations have been
        
      //std::cout << "Drawing generation " << nGenMax << "..." << std::endl;
      
      // number of sub-branches: 'coordination number' - 1
      int nBranches = coordNumber - 1;            
            
      // Draw the "mother branch" line in the current separator
      drawLine(x, y, x, y+len, group);
      
      // decrease the generation counter
      --nGenMax;
      
      // Then, draw the "children branches"...
      for (int nB=0; nB < nBranches; ++nB) {
                             
          SoSeparator* trSep = new SoSeparator;

          // ... and add them to the "mother" Separator
          group->addChild(trSep);
    
          // Declare a new transformation, one for each "child branch"
          SoTransform* myTransform = new SoTransform;
     
          // Set a fixed value for the branch angles...
          float theta = (-angle/2.) + ( angle/ (nBranches-1) ) * nB; 
          
          // set the values for center of rotation and rotation angle and axis    
          myTransform->rotation.setValue(SbVec3f(0,0,1), theta); 
          myTransform->center.setValue(SbVec3f(0,y+len,0));       
    
          // add the transformations to the "child" Separator
          trSep->addChild(myTransform);
     
          // we set the length for the "children branches": 2/3 of the  "mother branch"
          float branch_len = len * 0.66;
     
          // RECURSIVE CALL
          drawTree(x, y+len, branch_len, coordNumber, nGenMax, angle, stroke, trSep);    
      } // end of loop over sub-branches   
    } // stop the recursion    
} // end of DrawTree()


void drawLatticeOrigin(float x, float y, float len, int coordNumber, int nGenMax, float angle, float stroke, SoGroup* group) 
{
    // number of sub-branches == 'coordination number'
    int nBranches = coordNumber;        
    
    // decrease the generation counter
    --nGenMax;
    
      // Then, draw the "children branches"...
      for (int nB=0; nB < nBranches; ++nB) {
                             
          SoSeparator* trSep = new SoSeparator;

          // ... and add them to the "mother" Separator
          group->addChild(trSep);
    
          // Declare a new transformation, one for each "child branch"
          SoTransform* myTransform = new SoTransform;
     
          // Set a fixed value for the branch angles...
          //float theta = (-angle/2.) + ( angle/ (nBranches-1) ) * nB; 
          float theta = 2*M_PI/nBranches * nB;
          
          // set the values for center of rotation and rotation angle and axis    
          myTransform->rotation.setValue(SbVec3f(0,0,1), theta); 
          myTransform->center.setValue(SbVec3f(0,y+len,0));       
    
          // add the transformations to the "child" Separator
          trSep->addChild(myTransform);
     
          // we set the length for the "children branches": 2/3 of the  "mother branch"
          //float branch_len = len * 0.66;
     
          // RECURSIVE CALL
          drawTree(x, y+len, len, coordNumber, nGenMax, angle, stroke, trSep);    
      } // end of loop over sub-branches   
} // end of DrawFirstGeneration()



//______________________________
int main(int argc, char ** argv)
{
    
   /* -----------------------------
    * k-Caley Tree default settings
    * -----------------------------
    */
   // the 'coordination number' (or 'z')
   int coordNumber = 3;
   // number of generations
   int nGen = 10;
   // angle of the circular sector 
   // occupied by each generation
   float genSector = M_PI/2.; // 1.5708 rad
   // length of the lines of the first generation
   float len = 200.;
   // line weight 
   float stroke = 10.;
   // draw a 'tree shape'
   bool drawTreeShape = false;
   /* ----------------------------- */
  
  
   // take settings from command-line
   for (int i=1; i<argc; i++) {
       std::string thisArg(argv[i]);
       
       if (thisArg=="-z") {
           std::istringstream ss(argv[i+1]);
           if (!(ss >> coordNumber))
               std::cerr << "Invalid number " << argv[i+1] << "\n";
           
           std::copy(argv+i+1, argv+argc, argv+1);
           argc -= 1;
           i    -= 1;
       } else if (thisArg=="-n") {
           std::istringstream ss(argv[i+1]);
           if (!(ss >> nGen))
               std::cerr << "Invalid number " << argv[i+1] << "\n";
           
           std::copy(argv+i+1, argv+argc, argv+1);
           argc -= 1;
           i    -= 1;
       } else if (thisArg=="-a") {
           std::istringstream ss(argv[i+1]);
           if (!(ss >> genSector))
               std::cerr << "Invalid number " << argv[i+1] << "\n";
           
           std::copy(argv+i+1, argv+argc, argv+1);
           argc -= 1;
           i    -= 1;
       } else if (thisArg=="-l") {
           std::istringstream ss(argv[i+1]);
           if (!(ss >> len))
               std::cerr << "Invalid number " << argv[i+1] << "\n";
           
           std::copy(argv+i+1, argv+argc, argv+1);
           argc -= 1;
           i    -= 1;
       } else if (thisArg=="-tree") {
           drawTreeShape = true;
           std::copy(argv+i+1, argv+argc, argv+1);
           argc -= 1;
           i    -= 1;
       }  
   }
  
  std::cout << "\n--- Generating and visualizing a Cayley Tree ---\n" << std::endl;
  std::cout << "Using coordination number 'z': " << coordNumber << std::endl;
  std::cout << "Using generation number 'n': " << nGen << std::endl;
  std::cout << "Using sector angle 'a': " << genSector << std::endl;
  std::cout << "Using length of the 1st gen lines: 'l': " << len << std::endl;
  std::cout << "Drawing a 'tree shape': " << drawTreeShape << std::endl;
  std::cout << std::endl;
  std::cout << "You can modify these parameters using the -z, -n and -a command-line options.\nExample: ./CayleyTree -z 5 -n 4 -a 1.2 -l 100 -tree\n" << std::endl; 
  
  // Init the Qt windowing system
  // and get a pointer to the window 
  QApplication app(argc, argv);
  QWidget window;
  window.resize(400,400);
  
  SoQt::init(&window);
  
  // Set the main node for the "scene graph" 
  SoSeparator *root = new SoSeparator;
  root->ref();

  // Declare a Group node to store the tree.
  SoGroup* group = new SoGroup();
  
  
  // First, set up the line style...
  SoDrawStyle* style = new SoDrawStyle();
  //style->lineWidth.setValue(2.0f); //Line thickness
  style->lineWidth = stroke/5.;
  group->addChild(style);
  
  
  // Draw the Cayley Graph
  if (drawTreeShape)
      drawTree(0, 0, len, coordNumber, nGen, genSector, stroke, group);
  // draw a 'tree shape'
  else
      drawLatticeOrigin(0, 0, len, coordNumber, nGen, genSector, stroke, group);
      

  // Add to the scene graph the Group node containing the tree
  root->addChild(group);
  
  // Init the viewer and get a pointer to it
  SoQtExaminerViewer *b = new SoQtExaminerViewer(&window);

  // set background color to white (default is black)
  b->setBackgroundColor( SbColor(1, 1, 1) );
  
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



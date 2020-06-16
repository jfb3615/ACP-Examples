//
// world.cpp
//
// by Riccardo Maria BIANCHI  Jun. 2015
//
//

// Coin3D includes
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTexture2.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoComplexity.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoDrawStyle.h>
// SoQt includes
#include <Inventor/Qt/SoQt.h>
#include <Inventor/Qt/viewers/SoQtExaminerViewer.h>
// C++
#include <iostream>

  
int main(int argc, char ** argv)
{
	bool wireframe = false;

	// Get the command-line options
	for (int i=1; i<argc;i++) {
	    std::string thisArg(argv[i]);

//	    if (thisArg=="-w") {
//	      std::istringstream ss(argv[i + 1]);
//	      if (!(ss >> n_points))
//	        std::cerr << "Invalid number " << argv[i + 1] << '\n';
//	      //n_points = argv[i +1];
//	      std::copy(argv+i+1, argv+argc, argv+i);
//	      argc -= 1;
//	      i    -= 1;
//	    }
//	    else
	    	if (thisArg=="-w") {
			wireframe = true;
		  std::cout << "Using 'wireframe' rendering..." << std::endl;
		}
	  }



  // Init the SoQt library 
  // we get a pointer to a Qt window
  QWidget *window = SoQt::init("test");
  
  // init the main node of the "scene graph"
  SoSeparator *root = new SoSeparator;
  root->ref();

  // Two nodes for the two versions of the Earth
  SoSeparator *earth1 = new SoSeparator;
  SoSeparator *earth2 = new SoSeparator;
  


  /* Add a SoComplexity node only to group node 2
   * Default values:
   *   Complexity {
          type OBJECT_SPACE
          value 0.5
          textureQuality 0.5
   *   }
   */
  SoComplexity* complexity = new SoComplexity;
  complexity->value = 1.0;
  complexity->textureQuality = 1.0;
//  complexity->type = SoComplexity::BOUNDING_BOX;
  earth2->addChild(complexity);


  if (wireframe) {
	  /*
	   * Add a SoDrawStyle node to use 'wireframe' rendering
	   * Default values:
	   *   DrawStyle {
       *      style FILLED
       *      pointSize 0
       *      lineWidth 0
       *      linePattern 0xffff
       *   }
	   *
	   */
  SoDrawStyle* style1 = new SoDrawStyle;
  SoDrawStyle* style2 = new SoDrawStyle;
style1->style =  SoDrawStyle::LINES;
style2->style =  SoDrawStyle::LINES;

  earth1->addChild(style1);
  earth2->addChild(style2);
  }
  else{

  // A node for the texture
  SoTexture2 *texture_earth1 = new SoTexture2;
  SoTexture2 *texture_earth2 = new SoTexture2;

  // Name of texture file
  texture_earth1->filename = "world32k.jpg";
  texture_earth2->filename = "world32k.jpg";

  // Add texture to both groups
  earth1->addChild(texture_earth1);
  earth2->addChild(texture_earth2);
  }

  SoTranslation *earth2Pos = new SoTranslation;
  earth2Pos->translation.setValue(2.0, 0, 0);
  earth2->addChild(earth2Pos);

  // Create a sphere...
  SoSphere* sphere1 = new SoSphere;
  SoSphere* sphere2 = new SoSphere;
  sphere1->radius = 1.0;
  sphere2->radius = 1.0;

  // ...and add it to both group nodes
  earth1->addChild(sphere1);
  earth2->addChild(sphere2);
  
  // Add the Earth node to the root node
  root->addChild(earth1);
  root->addChild(earth2);
  
  
  // set the view with the root node and show it
  SoQtExaminerViewer *b = new SoQtExaminerViewer(window);
  b->setTitle("Textured Earths");
  b->setSceneGraph(root);
  b->show();
  
  // Start the Qt window and show it
  SoQt::show(window);
  
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

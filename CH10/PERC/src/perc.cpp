#include "Percolator.h"
#include "QatDataAnalysis/OptParse.h"
#include <Inventor/Qt/SoQt.h>
#include <Inventor/Qt/viewers/SoQtExaminerViewer.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoCamera.h>
#include <Inventor/nodes/SoLight.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/nodes/SoVertexProperty.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/manips/SoTransformBoxManip.h>
#include <Inventor/manips/SoHandleBoxManip.h>
#include <random>
#include <stdexcept>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <map>


SoSeparator *makeSphere(const SbVec3f & v) {
  SoSeparator *sep=new SoSeparator();

  SoTranslation *translation= new SoTranslation();
  translation->translation.setValue(v);
  sep->addChild(translation);

  SoSphere *sphere = new SoSphere;
  sep->addChild(sphere);

  return sep;
}

struct LatticeVector {
  SbVec3f a0;
  SbVec3f a1;
  SbVec3f a2;
};

int main(int argc, char ** argv)
{
  std::string usage= "usage " + std::string(argv[0]) + " [L=val/def=20] [p=val/def=0.31] -handlebox ";
  bool handleBox=false;
  unsigned int L=20;
  double p=0.31;
  NumericInput input;
  input.declare("L", "Lattice size", L);
  input.declare("p", "site probability", p);
  try {
    input.optParse(argc, argv);
  }
  catch (const std::exception & e) {
    std::cerr << "Error parsing command line" << std::endl;
    std::cerr << usage << std::endl;
    std::cerr << input.usage() << std::endl;
    exit(0);
  }
  if (argc==2) {
    if (argv[1]==std::string("-handlebox")) {
      handleBox=true;
    }
    else {
      std::cerr << "Error parsing command line" << std::endl;
      std::cerr << usage << std::endl;
      std::cerr << input.usage() << std::endl;
      exit(0);
    }
  }
  else if (argc>2) {
    std::cerr << "Error parsing command line" << std::endl;
    std::cerr << usage << std::endl;
    std::cerr << input.usage() << std::endl;
    exit(0);
  }
  
  L = (unsigned int ) (0.5 + input.getByName("L"));
  p = input.getByName("p");
  Percolator perc(L,L,L,p);
  perc.next();
  perc.cluster();

  // Make a main window:
  QWidget * mainwin = SoQt::init("Molecular Collisions");


  // The root of a scene graph
  SoSeparator *root = new SoSeparator;
  root->ref();

  // 
  std::map<unsigned int, SoSeparator *> sepMap;
  for (unsigned int i=0;i<L;i++) {
    for (unsigned int j=0;j<L;j++) {
      for (unsigned int k=0;k<L;k++) {
	
	int id=perc.getClusterId(i,j,k);
	if (1) {
	  unsigned int colorid=perc.getColor(id);
	  auto s=sepMap.find(colorid);
	  if (s==sepMap.end()) {
	    s=sepMap.insert(sepMap.begin(),std::make_pair(colorid,new SoSeparator));
	    SoSeparator *sep=(*s).second;
	    root->addChild(sep);


	    if (handleBox) sep->addChild(new SoHandleBoxManip());
	    
	    double red   = (colorid & 0x000000FF)/256.0;
	    double green = ((colorid & 0x0000FF00)>>8)/256.0;
	    double blue  = ((colorid & 0x00FF0000)>>16)/256.0;
	    SoMaterial *material = new SoMaterial;
	    material->diffuseColor.setValue(red,green, blue);
	    (*s).second->addChild(material);
	  }

	  SoSeparator *sep=(*s).second;
	  sep->addChild(makeSphere(SbVec3f(5*i, 5*j, 5*k)));

	}
	else {
	  root->addChild(makeSphere(SbVec3f(5*i, 5*j, 5*k)));
	}
      }  
    }  
  }

  
  SoQtExaminerViewer * eviewer = new SoQtExaminerViewer(mainwin);
  eviewer->setSceneGraph(root);
  eviewer->show();
  
  // Pop up the main window.
  SoQt::show(mainwin);
  // Loop until exit.
  SoQt::mainLoop();

  // Clean up resources.
  delete eviewer;
  root->unref();
}

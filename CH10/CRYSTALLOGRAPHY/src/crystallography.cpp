#include <Inventor/Qt/SoQt.h>
#include <Inventor/Qt/viewers/SoQtExaminerViewer.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/nodes/SoVertexProperty.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoCone.h>
#include <random>
#include <stdexcept>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <cmath>
// This program displays several crystal lattices, namely:
// 0) Cubic
// 1) Tetragonal
// 2) Orthorhombic
// 3) Rombohedral
// 4) Hexagonal

// Make a sphere at position v:
SoSeparator *makeSphere(const SbVec3f & v) {
  SoSeparator *sep=new SoSeparator();

  SoMaterial *material = new SoMaterial;
  material->diffuseColor.setValue(1,0,0);
  sep->addChild(material);

  SoTranslation *translation= new SoTranslation();
  translation->translation.setValue(v);
  sep->addChild(translation);

  SoSphere *sphere = new SoSphere;
  sep->addChild(sphere);

  return sep;
}

// The lattice is defined by lattice vectors, also called primitive vectors:
struct LatticeVector {
  SbVec3f a0;
  SbVec3f a1;
  SbVec3f a2;
};

int main(int argc, char **argv)
{

  std::string usage=std::string("usage: ") + argv[0] + " [val[0-4]]";
  
  int type=0;
  if (argc>2) {
    std::cerr << usage << std::endl;
    return 1;
  }
  else if (argc==2) {
    type=atoi(argv[1]);
    if (type<0 || type>4) {
      std::cerr << usage << std::endl;
      return 2;
    }
  }
  



  double a=3,c=4;
  LatticeVector Cubic       ={SbVec3f(5,0,0), SbVec3f(0,5,0), SbVec3f(0,0,5)};
  LatticeVector Tetragonal  ={SbVec3f(5,0,0), SbVec3f(0,5,0), SbVec3f(0,0,10)};
  LatticeVector Orthorhombic={SbVec3f(5,0,0), SbVec3f(0,15,0), SbVec3f(0,0,10)};
  LatticeVector Rhombohedral={SbVec3f(a,0,c), 0.5*SbVec3f(-a, sqrt(3)*a, 2*c), 0.5*SbVec3f(-a, -sqrt(3)*a, 2*c)};
  LatticeVector Hexagonal   ={0.5*SbVec3f(sqrt(3)*a, -a, 0), SbVec3f(0,a,0), SbVec3f(0,0,c)};

  LatticeVector  lSet[]= {Cubic, Tetragonal, Orthorhombic, Rhombohedral, Hexagonal};
  std::string    lName[]={"Cubic", "Tetragonal", "Orthorhombic", "Rhombohedral", "Hexagonal"};

  // Make a main window:
  QWidget * mainwin = SoQt::init(lName[type].c_str());

  // The root of a scene graph
  SoSeparator *root = new SoSeparator;
  root->ref();


  const LatticeVector & lv=lSet[type];
  for (int i=0;i<10;i++) {
    for (int j=0;j<10;j++) {
      for (int k=0;k<10;k++) {
	root->addChild(makeSphere(i*lv.a0 + j*lv.a1+ k*lv.a2));
      }  
    }  
  }

  
  SoQtExaminerViewer * eviewer = new SoQtExaminerViewer(mainwin);
  eviewer->setTitle(lName[type].c_str());
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

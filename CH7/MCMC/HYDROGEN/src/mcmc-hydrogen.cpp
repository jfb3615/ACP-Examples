#include "QatDataAnalysis/OptParse.h"
#include <Inventor/Qt/SoQt.h>
#include <Inventor/Qt/viewers/SoQtExaminerViewer.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>
#include <Inventor/nodes/SoCamera.h>
#include <Inventor/nodes/SoRotation.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/nodes/SoVertexProperty.h>
#include <QApplication>
#include <QAction>
#include "Psi2Hydrogen.h"
#include "Eigen/Dense"
#include <iostream>
#include <vector>
#include <sstream>
#include <random>

using namespace std;

#define RBD_EPSILON 1E-4

class HydrogenAtomMarkovChain {


public:
  
  HydrogenAtomMarkovChain(int n, int l, int m, double sigma=1.0):
    psi(n,l,m),
    seed1(999),
    seed2(345),
    gauss(0,sigma)

  {
  }

  double move(Eigen::VectorXd & v, double prob=0.0) {
    
    Eigen::VectorXd vProp=v;
    for (int d=0;d<vProp.rows();d++) {
      vProp[d]+=gauss(engine);
    }
    
    // The argument to the hydrogen wavefunction class is in spherical 
    // polar coordinates:
    Genfun::Argument r(3);
    r[0]=sqrt(vProp[0]*vProp[0]+vProp[1]*vProp[1]+vProp[2]*vProp[2]);
    r[1]=vProp[2]/r[0];
    r[2]=atan2(vProp[1],vProp[0]);
    
    
    double newProb=psi(r);
    if (newProb>prob) {
      v=vProp;
      return newProb;
    }
    else {
      if (flat(engine) < newProb/prob) {
	v=vProp;
	return newProb;
      }
      else {
	return prob;
      }
    }
  }
  
private:
  
  Genfun::Psi2Hydrogen psi;   // the Hydrogen PDF
  unsigned int seed1;
  unsigned int seed2;
  mt19937  engine;
  uniform_real_distribution<double> flat;
  normal_distribution<double>       gauss;
};


int main (int argc, char ** argv) {

  std::string usage = std::string("usage: ") + argv[0] + "[NPOINTS=val{def=10000}] [N=val{def=0}] [L=val{def=0}] [M=val{def=0}] [fSigma=val{def=1.0}";
  NumericInput numeric;
  numeric.declare("NPOINTS", "Number of points in the Markov Chain", 10000);
  numeric.declare("N",       "Principal Quantum Number", 1);
  numeric.declare("L",       "Angular Momentum Quantum Number", 0);
  numeric.declare("M",       "Magnetic Quantum Number", 0);
  numeric.declare("fSigma",  "Under/over scale the proposal distribution", 1.0);
  numeric.optParse(argc, argv);
  
  if (argc>1) {
    std::cout << usage << std::endl;
    std::cout << numeric.usage() << std::endl;
    exit(0);
  }
  unsigned int NPOINTS = (unsigned int)(0.5+numeric.getByName("NPOINTS"));
  unsigned int N      = (unsigned int)(0.5+numeric.getByName("N"));
  unsigned int L      = (unsigned int)(0.5+numeric.getByName("L"));
  unsigned int M      = (unsigned int)(0.5+numeric.getByName("M"));
  double   fSigma     = numeric.getByName("fSigma");  
  
  
  Eigen::VectorXd v(3);
  v[0]=0;
  v[1]=0;
  v[2]=0;

  std::vector<Eigen::VectorXd> reboundCollection;
  HydrogenAtomMarkovChain chain(N,L,M,fSigma*N);
  double prob=0;

  for(unsigned int i=0;i<NPOINTS;i++) 
  {
    reboundCollection.push_back(v);
    prob=chain.move(v,prob);
  }  


  //---------------------------------------------------------
  // Now that we have reboundCollection, start visualizing!

  std::ostringstream nameStream;
  nameStream << "Hydrogen " << N << " " << L << " " << M << std::endl;
    
  QWidget * mainwin = SoQt::init(nameStream.str().c_str() );

  SoSeparator *root = new SoSeparator;
  root->ref();
  
    
  SoMaterial *cubeMaterial = new SoMaterial;
  cubeMaterial->diffuseColor.setValue(0.0, 0.0, 0.0);
  root->addChild(cubeMaterial);

  SoDrawStyle *cubeDrawStyle=new SoDrawStyle();
  cubeDrawStyle->style=SoDrawStyle::LINES;
  root->addChild(cubeDrawStyle);

  SoLightModel *cubeLightModel = new SoLightModel;
  cubeLightModel->model=SoLightModel::BASE_COLOR;
  root->addChild(cubeLightModel);

  SoCube *cube = new SoCube();
  cube->width=30;
  cube->height=30;
  cube->depth=30;
  root->addChild(cube);

  SoMaterial *walkMaterial = new SoMaterial;
  walkMaterial->diffuseColor.setValue(0, 0, 0);
  root->addChild(walkMaterial);

  SoVertexProperty *property = new SoVertexProperty();
  for (unsigned int i=0;i<reboundCollection.size();i++) {
    const Eigen::VectorXd A = reboundCollection[i];
    property->vertex.set1Value(i,A[0],A[2],A[1]);
  }

  SoNode *set=NULL;
  if (1) {
    SoPointSet *pset = new SoPointSet;
    pset->vertexProperty=property;
    set=pset;
  }
  else {
    SoLineSet *lset = new SoLineSet;
    lset->vertexProperty=property;
    set=lset;
  }
  root->addChild(set);

  // Use one of the convenient SoQt viewer classes.
  SoQtExaminerViewer * eviewer = new SoQtExaminerViewer(mainwin,nameStream.str().c_str());
  //  eviewer->setTransparencyType(SoGLRenderAction::SCREEN_DOOR);
  eviewer->setSceneGraph(root);
  eviewer->setBackgroundColor(SbColor(1.0,1.0,1.0));
  eviewer->getCamera()->position.setValue(0,0,115);
  eviewer->show();
  
  // Pop up the main window.
  SoQt::show(mainwin);
  // Loop until exit.
  SoQt::mainLoop();

  // Clean up resources.
  delete eviewer;
  root->unref();
  



  return 0;
}






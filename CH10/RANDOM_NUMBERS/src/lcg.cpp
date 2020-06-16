/*
 *
 *****************************
 Linear congruential generator
 *****************************
 *
 * A linear congruential generator is a simple pseudo-random number generator,
 * which use a recurrence relation of this form:
 *
 *    r_{n+1} = ( a * r_{n} + c ) * (modulo m)
 *
 * Where:
 *
 * - "m" is the "modulus"
 * - "c" is the "increment"
 * - "a" is the "multiplier"
 *
 * and where:
 *
 * - r0 in [0, m] is the "seed": it starts the sequence
 * - r1, r2, r3, ..., are the generated random numbers
 * - a, c, m are constant parameters
 *
 * The generator produces a uniform distribution of integers from 0 to m − 1,
 * from a careful choice of the constant parameters a, c and m.
 *
 * FEATURES:
 *
 * - easy to understand and code
 * - predictible: we get the same sequence of numbers using the same seed
 *
 * PROBLEMS:
 *
 * - r_{n} and r_{n+1} are not independent, as true random numbers should be
 *
 * (See: http://rosettacode.org/wiki/Linear_congruential_generator)
 */

// Coin3D / Open Inventor includes
#include <Inventor/Qt/SoQt.h>
#include <Inventor/Qt/viewers/SoQtExaminerViewer.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoCamera.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoLightModel.h>

// Qt includes
#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QToolBar>
#include <QAction>
#include <QDebug>

// C++ includes
#include <cstdlib>
#include <iostream>
#include <string>
#include <cmath>
#include <sstream>
#include <random>
using namespace std;

// Instantiate a random number generator identical to the deficient RANDU
linear_congruential_engine<uint_fast32_t,     65539,    0,0X80000000> randu(1);





/*
 * Main program
 */
int main(int argc, char * * argv) 
{
  const std::string usage=std::string(argv[0]) + " [-n N] [-f]";
  

  int n_points=100000;
  for (int i=1; i<argc;i++) {
    std::string thisArg(argv[i]);

    if (thisArg=="-n") {
      std::istringstream ss(argv[i + 1]);
      if (!(ss >> n_points))
        std::cerr << "Invalid number " << argv[i + 1] << '\n';
      //n_points = argv[i +1];
      std::cout << "Generating " << n_points << " points..." << std::endl;
      std::copy(argv+i+1, argv+argc, argv+i);
      argc -= 1;
      i    -= 1;
    } 
  }

  //
  // Initialize graphics:
  //
  QApplication     app(argc,argv);
  QMainWindow mainwin;
  mainwin.setMinimumWidth(800);
  mainwin.setMinimumHeight(800);
  SoQt::init(&mainwin);

  SoSeparator *root = new SoSeparator;
  root->ref();

  //
  // Make the three axes: x, y, z
  //

  // a new Separator to gather the three axes
  SoSeparator *aSep=new SoSeparator();
  root->addChild(aSep);

  // a loop to create the three axes
  for (int i=0;i<3;i++) {

    // coordinates for the starting and ending points of the axes
    SoCoordinate3 *arrow = new SoCoordinate3();
    arrow->point.set1Value(0,0,0,0);
    
    // (1,1,0,0), (1,0,1,0) or (1,0,0,1)
    arrow->point.set1Value(1,i==0,i==1,i==2); 
    aSep->addChild(arrow);

    // drawing style
    SoDrawStyle *ds= new SoDrawStyle();
    ds->style=SoDrawStyle::LINES;
    ds->lineWidth=3;
    aSep->addChild(ds);

    // material/color for the axes
    SoMaterial *aMaterial = new SoMaterial();
    // (1,0,0), (0,1,0) or (0,0,1)
    aMaterial->diffuseColor.setValue(i==0,i==1,i==2); 
    aSep->addChild(aMaterial);

    // a LineSet, which uses the Coordinates to draw the axes lines
    SoLineSet *line = new SoLineSet();
    line->numVertices=2;
    aSep->addChild(line);
  }


  // Adding the Coordinates and the PointSet to the root separator
  SoCoordinate3 *coordinates = new SoCoordinate3();
  root->addChild(coordinates);
  SoPointSet *pointSet =new SoPointSet;
  root->addChild(pointSet);


  // Fill the coordinates with a vector of random numbers:
  for (int i=0;i<n_points;i++) {
    // generate new random coordinates
    double X = double(randu())/randu.max();
    double Y = double(randu())/randu.max();
    double Z = double(randu())/randu.max();
    coordinates->point.set1Value(i,X,Y,Z);
    pointSet->numPoints=i+1;
  }


  
  //
  // Make an examiner viewer:
  //
  SoQtExaminerViewer * b = new SoQtExaminerViewer();

  //
  // Make a tool bar with a quit button, and set the
  // Keyboard accelerator:
  //
  QToolBar *toolBar=mainwin.addToolBar("Tools");
  QAction  *quitAction=toolBar->addAction("Quit");
  quitAction->setShortcut(QKeySequence("q"));
  QObject::connect(quitAction, SIGNAL(triggered()), &app, SLOT(quit()));
  
  //
  // Lay out in a box (this allows to add some graphs later if
  // needed):
  //
  QVBoxLayout *layout=new QVBoxLayout();
  layout->addWidget(b->getWidget());
  QWidget *widget=new QWidget();
  widget->setLayout(layout);
  mainwin.setCentralWidget(widget);
  b->setSceneGraph(root);
  
  //
  // Position the camera (overriding some inconvenient defaults)
  //
  b->getCamera()->position.setValue(1,1,6);
  b->getCamera()->focalDistance=6;
  b->saveHomePosition();
  b->viewAll();

  // Pop up the main window
  SoQt::show(&mainwin);

  // Loop until exit.
  SoQt::mainLoop();

  // Clean up resources.
  delete b;
  root->unref();

  return 1;
}

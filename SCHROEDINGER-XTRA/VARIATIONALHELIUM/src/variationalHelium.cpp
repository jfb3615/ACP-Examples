#include "HeliumObjective.h"
#include "QatPlotWidgets/PlotView.h"
#include "QatPlotting/PlotStream.h"
#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include "QatDataAnalysis/OptParse.h"
#include "QatDataModeling/MinuitMinimizer.h"
#include "QatGenericFunctions/Square.h"
#include "QatGenericFunctions/Parameter.h"
#include "QatPlotting/PlotFunction1D.h"
#include "Eigen/Dense"

// Follow the numbered steps to understand this program:
using namespace Eigen;
using namespace std;
using namespace Genfun;




int main (int argc, char * * argv) {

  // Automatically generated:-------------------------:

  std::string usage= std::string("usage: ") + argv[0] + "N=val"; 
  NumericInput input;
  input.declare("N", "Number of components", 4);
  try {
    input.optParse(argc, argv);
  }
  catch (exception &) {
    std::cout << usage         << std::endl;
    std::cout << input.usage() << std::endl;
    exit(0);
  }

  if (argc!=1) {
    std::cout << usage << std::endl;
  }

  unsigned int N=(unsigned int) (0.5+input.getByName("N"));

  QApplication     app(argc,argv);
  
  QMainWindow window;
  QToolBar *toolBar=window.addToolBar("Tools");
  QAction  *nextAction=toolBar->addAction("Next");
  
  nextAction->setShortcut(QKeySequence("n"));
  
  QObject::connect(nextAction, SIGNAL(triggered()), &app, SLOT(quit()));

  PRectF rect;
  rect.setXmin(0.0);
  rect.setXmax(2.0);
  rect.setYmin(0.0);
  rect.setYmax(2.0);
  
  PlotView view(rect);
  window.setCentralWidget(&view);

  PlotStream titleStream(view.titleTextEdit());
  titleStream << PlotStream::Clear()
	      << PlotStream::Center() 
	      << PlotStream::Family("Sans Serif") 
	      << PlotStream::Size(12)
	      << "Helium wavefunction, variational principle"
	      << PlotStream::EndP();
  
  
  PlotStream yLabelStream(view.yLabelTextEdit());
  yLabelStream << PlotStream::Clear()
	       << PlotStream::Center()
	       << PlotStream::Family("Sans Serif")
	       << PlotStream::Size(16)
	       << "|"
	       << PlotStream::Family("Symbol")
	       << "Y"
	       << PlotStream::Family("Sans Serif")
	       << "(x)|"
	       << PlotStream::Super()
	       << "2"
	       << PlotStream::Normal()
	       << PlotStream::EndP();
  
  PlotStream xLabelStream(view.xLabelTextEdit());
  xLabelStream << PlotStream::Clear()
	       << PlotStream::Center()
	       << PlotStream::Family("Sans Serif")
	       << PlotStream::Size(16)
	       << "x"
	       << PlotStream::EndP();

  PlotStream statStream(view.statTextEdit());

  
  
  HeliumObjective *oldObjective=NULL;
  HeliumObjective *newObjective=new HeliumObjective(N,oldObjective);
  for (int i=0;i<10000;i++) {  

    //
    // 2. Create a minimizer to minimize over the parameters:
    //
    bool verbose=false;
    MinuitMinimizer minimizer(verbose);
    minimizer.addStatistic(newObjective);
    for (unsigned int i=0;i<N;i++) {
      minimizer.addParameter(newObjective->getAlpha(i));
      if (i!=0) minimizer.addParameter(newObjective->getC(i));
    }
    minimizer.minimize();
    newObjective->normalize();

    //
    // Print the result to the viewer window.
    //
    std::ostringstream stream;
    stream << "\nE=" << newObjective->energy() << "\n";

    statStream << PlotStream::Clear()
	       << PlotStream::Center()
	       << PlotStream::Family("Sans Serif")
	       << PlotStream::Size(16)
	       << stream.str() <<PlotStream::EndP();
    //
    // 3.  Plot the result:
    //
    PlotFunction1D p=Genfun::Square()(newObjective->wavefunction());
    
    view.add(&p);
    
    PlotFunction1D::Properties prop;
    prop.pen.setWidth(4);
    prop.pen.setColor("darkRed");
    p.setProperties(prop);
    view.recreate();
    
    
    view.show();
    window.show();
    app.exec();
    delete oldObjective;
    oldObjective=newObjective;
    newObjective = new HeliumObjective(N,oldObjective);
    view.clear();
  }
  delete newObjective;
  delete oldObjective;
  return 1;
}


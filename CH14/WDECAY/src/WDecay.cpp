#include "Spacetime/Spinor.h"
#include "Spacetime/Rotation.h"
#include "QatPlotWidgets/PlotView.h"
#include "QatPlotting/PlotStream.h"
#include "QatPlotting/PlotFunction1D.h"
#include "QatGenericFunctions/F1D.h"
#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <cstdlib>
#include <iostream>
#include <string>


double ASQ (double cosTheta)  {
  Spinor<3> chiInitial={1,0,0};
  Spinor<3> chiFinal  ={0,0,1};
  Rotation R(ThreeVector(0,-acos(cosTheta),0));
  return 3.0/4.0/M_PI*norm(((R*chiFinal).adjoint()*chiInitial)(0) );
};


int main (int argc, char * * argv) {

  // Automatically generated:-------------------------:

  std::string usage= std::string("usage: ") + argv[0]; 
  if (argc!=1) {
    std::cout << usage << std::endl;
  }


  QApplication     app(argc,argv);
  
  QMainWindow window;
  QToolBar *toolBar=window.addToolBar("Tools");
  QAction  *quitAction=toolBar->addAction("Quit");
  
  quitAction->setShortcut(QKeySequence("q"));
  
  QObject::connect(quitAction, SIGNAL(triggered()), &app, SLOT(quit()));
  
  PRectF rect;
  rect.setXmin(-1.0 );
  rect.setXmax( 1.0);
  rect.setYmin(0.0);
  rect.setYmax(0.5);


  PlotView view(rect);
  window.setCentralWidget(&view);

  PlotFunction1D p=Genfun::F1D(ASQ);
  PlotFunction1D::Properties prop;
  prop.pen.setWidth(3);
  p.setProperties(prop);
  view.add(&p);
  
  PlotStream titleStream(view.titleTextEdit());
  titleStream << PlotStream::Clear()
	      << PlotStream::Center() 
	      << PlotStream::Family("Sans Serif") 
	      << PlotStream::Size(16)
	      << "W→e⁻ν̅"
	      << PlotStream::EndP();
  
  PlotStream xLabelStream(view.xLabelTextEdit());
  xLabelStream << PlotStream::Clear()
	       << PlotStream::Center()
	       << PlotStream::Family("Sans Serif")
	       << PlotStream::Size(16)
	       << "cosθ"
	       << PlotStream::EndP();
  
  PlotStream yLabelStream(view.yLabelTextEdit());
  yLabelStream << PlotStream::Clear()
	       << PlotStream::Center()
	       << PlotStream::Family("Sans Serif")
	       << PlotStream::Size(12)
	       << "|A(cosΘ)|²"
	       << PlotStream::EndP();
  
  
  view.show();
  window.show();
  app.exec();
  return 1;
}


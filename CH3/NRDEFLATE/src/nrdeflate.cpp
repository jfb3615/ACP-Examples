#include "QatPlotWidgets/PlotView.h"
#include "QatPlotting/PlotStream.h"
#include "QatGenericFunctions/Variable.h"
#include "QatPlotting/PlotFunction1D.h"
#include "QatPlotting/PlotPoint.h"
#include "QatPlotting/PlotProfile.h"
#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <cstdlib>
#include <iostream>
#include <string>

// Plot labelling:
const std::vector<std::string> title={"No roots found",
				      "One root found",
				      "Two roots found",
				      "Three roots found",
				      "Four roots found",
				      "Five roots found"};


const Genfun::AbsFunction *f{nullptr};
PlotProfile    prf;          // Set of zeros.
PlotFunction1D *P {nullptr}; // Original function
PlotFunction1D *P1{nullptr}; // Deflated function



double newtonRaphson(double x, Genfun::GENFUNCTION P) { 
  double x1=x;
  while (1) {
    double deltaX=-P(x)/P.prime()(x); 
    x+=deltaX;
    if (float(x1)==float(x)) break; 
    x1=x;
  }
  return x; 
}



class SignalCatcher: public QObject{
  Q_OBJECT
public:
  SignalCatcher(PlotView *pV):pV(pV){}
  PlotView *pV;
public slots:
  void next() {
    static int i=0;
    pV->clear();


    PlotStream titleStream(pV->titleTextEdit());
    titleStream << PlotStream::Clear()
		<< PlotStream::Center() 
		<< PlotStream::Family("Sans Serif") 
		<< PlotStream::Size(16)
		<< title[i+1]
		<< PlotStream::EndP();
    i++;
    if (i>5) exit(0);

    double x = newtonRaphson(-1.0, *f);
    prf.addPoint(x,0);
    Genfun::Variable X;
    Genfun::GENFUNCTION F1 = (*f)/(X-x);
    delete P1;
    P1=new PlotFunction1D(F1);
    {
      PlotFunction1D::Properties prop;
      prop.pen.setWidth(2);
      prop.pen.setStyle(Qt::DashLine);
      P1->setProperties(prop);
    }
    pV->add(P);
    pV->add(P1);
    pV->add(&prf);
    
    f=F1.clone();
   

  }
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
  QAction  *nextAction=toolBar->addAction("Next");
  
  nextAction->setShortcut(QKeySequence("n"));
  
  PRectF rect;
  rect.setXmin(0.0);
  rect.setXmax(5.0);
  rect.setYmin(-2.0);
  rect.setYmax(2.0);
  


  
  PlotView view(rect);
  view.setXZero(false);
  view.setGrid(false);
  window.setCentralWidget(&view);

  SignalCatcher signalCatcher(&view);
  QObject::connect(nextAction, SIGNAL(triggered()), &signalCatcher, SLOT(next()));

  using namespace Genfun;
  Variable X;
  GENFUNCTION F=(X-1)*(X-2)*(X-3)*(X-M_PI)*(X-4);
  P=new PlotFunction1D(F);
  f=&F;
  view.add(P);

  {
    PlotFunction1D::Properties prop;
    prop.pen.setWidth(3);
    P->setProperties(prop);
  }

  {
    PlotProfile::Properties prop;
    prop.brush.setStyle(Qt::SolidPattern);
    prop.brush.setColor("darkBlue");
    prop.symbolSize=10;
    prf.setProperties(prop);
  }
				  


  
  PlotStream titleStream(view.titleTextEdit());
  titleStream << PlotStream::Clear()
	      << PlotStream::Center() 
	      << PlotStream::Family("Sans Serif") 
	      << PlotStream::Size(16)
	      << title[0]
	      << PlotStream::EndP();
  

  PlotStream xLabelStream(view.xLabelTextEdit());
  xLabelStream << PlotStream::Clear()
	       << PlotStream::Center()
	       << PlotStream::Family("Sans Serif")
	       << PlotStream::Size(16)
	       << "x"
	       << PlotStream::EndP();
  
  PlotStream yLabelStream(view.yLabelTextEdit());
  yLabelStream << PlotStream::Clear()
	       << PlotStream::Center()
	       << PlotStream::Family("Sans Serif")
	       << PlotStream::Size(16)
	       << "y=f(x)"
	       << PlotStream::EndP();
  
  view.show();
  window.show();
  app.exec();

  return 1;
}
#include "nrdeflate.moc"



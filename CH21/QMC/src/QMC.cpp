#include "Simulator.h"
#include "QatPlotWidgets/PlotView.h"
#include "QatPlotWidgets/MultipleViewWindow.h"
#include "QatPlotting/PlotProfile.h"
#include "QatPlotting/PlotFunction1D.h"
#include "QatPlotting/PlotStream.h"
#include "QatPlotting/PlotHist1D.h"
#include "QatDataAnalysis/OptParse.h"
#include "QatDataAnalysis/Hist1D.h"
#include "QatGenericFunctions/Variable.h"
#include "QatGenericFunctions/Exp.h"

#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>
#include <random>


using namespace Genfun;
using namespace std;



int main (int argc, char * * argv) {
  random_device dev;
  mt19937 engine(dev());
  normal_distribution<double> gauss(0,2.0);
  
  unsigned int NFRAMES=1000;
  unsigned int NPARTICLES=1000;
  string usage= string("usage: ") + argv[0] + " [NFRAMES=val/1000] [NPARTICLES=val/1000]"; 

  NumericInput input;
  input.declare("NFRAMES",    "Number of time frames", NFRAMES);
  input.declare("NPARTICLES", "Number of particles  ", NPARTICLES);
  try {
    input.optParse(argc, argv);
  }
  catch (const exception & e) {
    cerr << e.what() << endl;
    cerr << input.usage() << endl;
    cerr << usage << endl;
    exit (1);
  }
  if (argc!=1) {
    cerr << usage << endl;
    exit (2);
  }

  NFRAMES    = (unsigned int) (0.5+input.getByName("NFRAMES"));
  NPARTICLES = (unsigned int) (0.5+input.getByName("NPARTICLES"));
  
  QApplication     app(argc,argv);
  
  MultipleViewWindow window;
  QToolBar *toolBar=window.addToolBar("Tools");
  QAction  *nextAction=toolBar->addAction("Next");
  
  nextAction->setShortcut(QKeySequence("n"));
  
  QObject::connect(nextAction, SIGNAL(triggered()), &app, SLOT(quit()));
  
  PRectF rect;
  rect.setXmin(-20);
  rect.setXmax(20);
  rect.setYmin(0.0);
  rect.setYmax(NPARTICLES*1.1);
  

  PlotView view(rect);
  window.add(&view, "Wavefunction");

  PlotView timeDevView;

  rect.setXmin(0);
  rect.setXmax(NFRAMES);
  rect.setYmin(0.0);
  rect.setYmax(NPARTICLES*1.1);
  timeDevView.setRect(rect);

  window.add(&timeDevView, "TimeDev");
  
  PlotProfile timeDevProf;
  
  PlotStream titleStream(view.titleTextEdit());
  titleStream << PlotStream::Clear()
	      << PlotStream::Center() 
	      << PlotStream::Family("Sans Serif") 
	      << PlotStream::Size(16)
	      << PlotStream::EndP();
  
  
  PlotStream xLabelStream(view.xLabelTextEdit());
  xLabelStream << PlotStream::Clear()
	       << PlotStream::Center()
	       << PlotStream::Family("Sans Serif")
	       << PlotStream::Size(16)
	       << PlotStream::EndP();
  
  PlotStream yLabelStream(view.yLabelTextEdit());
  yLabelStream << PlotStream::Clear()
	       << PlotStream::Center()
	       << PlotStream::Family("Sans Serif")
	       << PlotStream::Size(16)
	       << PlotStream::EndP();

  vector<Simulator *> simVector(NPARTICLES);
  Variable X;
  GENFUNCTION U=X*X/2.0;

  {
    std::vector<std::thread *> threadVector(NPARTICLES);
    
    for (size_t i=0;i<NPARTICLES;i++) {
      simVector[i]=new Simulator(U,gauss(engine), NFRAMES);
      simVector[i]->simulate();
      threadVector[i]=new std::thread(&Simulator::simulate, simVector[i]);
    }
    for (size_t i=0;i<NPARTICLES;i++) {
      threadVector[i]->join();
      delete threadVector[i];
    }  
  }
  vector<Hist1D *> histVector(NFRAMES);
  for (size_t t=0;t<NFRAMES;t++) {
    histVector[t]=new Hist1D(200, -20, 20);
    for (size_t i=0;i<NPARTICLES;i++) {
      histVector[t]->accumulate(simVector[i]->position(t), exp(-simVector[i]->weight(t)));
    }
  }
  window.show();
  GENFUNCTION psi0=(1/sqrt(2*M_PI))*Exp()(-X*X/2.0);
  PlotFunction1D::Properties prop;
  prop.pen.setWidth(3.0);
  prop.pen.setColor("darkRed");

  std::vector<double> sumV;


  
  for (int t=0;t<int(NFRAMES);t++) {
    PlotFunction1D pPsi0=histVector[t]->sum()*histVector[t]->binWidth()*psi0;
    pPsi0.setProperties(prop);
    PlotHist1D p=*histVector[t];
    view.setRect(p.rectHint());
    view.add(&pPsi0);
    view.add(&p);
    timeDevView.add(&timeDevProf);

    sumV.push_back(histVector[t]->sum());
    timeDevProf.addPoint(t,sumV.back());
    timeDevProf.addPoint(-t,sumV.back());
    
    
    
    app.exec();
    view.clear();
    timeDevView.clear();
  }

  PlotHist1D p=*histVector.back();
  

  return 1;
}

 

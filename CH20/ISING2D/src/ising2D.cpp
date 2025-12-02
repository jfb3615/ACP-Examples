#include "IsingModelWidget.h"
#include "IsingModel.h"
#include "QatPlotWidgets/PlotView.h"
#include "QatPlotting/PlotStream.h"
#include "QatPlotting/PlotProfile.h"
#include "QatPlotting/PlotHist1D.h"
#include "QatDataAnalysis/Hist1D.h"
#include "QatDataAnalysis/OptParse.h"
#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QTimer>
#include <QAction>
#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>
#include <memory>

class Threader {

public:

  Threader(IsingModel *model,IsingModelWidget *window);
  
  PlotProfile      uProf;
  PlotProfile      tProf;
  PlotProfile      uVsTProf;

  PlotProfile      mProf;
  PlotProfile      mVsTProf;
  
  void evolve();
  void terminate() {finish=true;};
private:
  
  IsingModel       *model{nullptr};
  IsingModelWidget *window{nullptr};
  bool              finish{false};
};

Threader::Threader(IsingModel *model,IsingModelWidget *window):model(model),window(window)
{
  // Set some plot properties:
  {
    PlotProfile::Properties prop;
    prop.brush.setStyle(Qt::SolidPattern);
    prop.brush.setColor("darkRed");
    prop.pen.setColor("darkRed");
    tProf.setProperties(prop);
    prop.brush.setColor("darkBlue");
    prop.pen.setColor("darkBlue");

    uProf.setProperties(prop);
    uVsTProf.setProperties(prop);

    mProf.setProperties(prop);
    mVsTProf.setProperties(prop);

  }
  window->getUSeriesView()->add(&uProf);
  window->getUSeriesView()->add(&tProf);
  window->getUVsTView()->add(&uVsTProf);
  window->getMSeriesView()->add(&mProf);
  window->getMVsTView()->add(&mVsTProf);
 


  
}


void Threader::evolve() {
	       
  const double norm=model->NX()*model->NY();
  unsigned long int nIter=0;
  while (!finish) {
    model->next();
    unsigned int i=0, j=0;
    int delta=0;
    
    model->lastStep(i,j,delta);
    bool on=model->isUp(i,j);
    window->updateImage(i,j,on);
    nIter++;

    double E=model->U()/norm;
    double T=model->tau();
    double M=model->M()/norm;
    const unsigned int NSAMPLE=1000000;
    if (window->getAcquire() && !(nIter%NSAMPLE)) {
      double normIter=nIter/NSAMPLE;
      window->setIterations(nIter/NSAMPLE);

      uProf.addPoint(normIter, E);
      tProf.addPoint(normIter, T);
      uVsTProf.addPoint(T, E); 

      mProf.addPoint(normIter,M);
      mVsTProf.addPoint(T, M); 
      
    }
 }
}

int main (int argc, char * * argv) {

  // Automatically generated:-------------------------:

  std::string usage= std::string("usage: ") + argv[0] + " [N=val]"; 
  NumericInput input;
  input.declare ("N", "Lattice dimension", 400);
  try {
    input.optParse(argc, argv);
  }
  catch (std::exception & ) {
    std::cout << "usage" << std::endl;
    std::cout << input.usage() << std::endl;
    exit(0);
  }
  if (argc!=1) {
    std::cout << usage << std::endl;
    exit(0);
  }
  
  unsigned int N = (unsigned int) (0.5+input.getByName("N"));
  QApplication     app(argc,argv);
  IsingModel       model(N,N,1.0);
  IsingModelWidget window(&model);

  Threader threader(&model,&window);
  



  std::thread evolveThread (&Threader::evolve,&threader); 

 

  QTimer *timer=new QTimer;
  timer->setInterval(100); // milliseconds
  
  
  QObject::connect(timer, SIGNAL(timeout()), &window, SLOT(updatePixmap()));
  timer->start();

  window.show();
  app.exec();
  
  threader.terminate();
  evolveThread.join();
  return 0;
}


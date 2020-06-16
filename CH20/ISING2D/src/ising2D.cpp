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
struct Threader {
  IsingModel       *model;
  IsingModelWidget *window;
  PlotProfile      *uProf;
  PlotProfile      *tProf;
  PlotProfile      *uVsTProf;

  PlotProfile      *mProf;
  PlotProfile      *mVsTProf;

};
bool finish=false;
void evolve(Threader *threader) {
	       
  const double norm=threader->model->NX()*threader->model->NY();
  unsigned long int nIter=0;
  while (!finish) {
    threader->model->next();
    unsigned int i=0, j=0;
    int delta=0;
    
    threader->model->lastStep(i,j,delta);
    bool on=threader->model->isUp(i,j);
    threader->window->updateImage(i,j,on);
    nIter++;

    double E=threader->model->U()/norm;
    double T=threader->model->tau();
    double M=threader->model->M()/norm;
    const unsigned int NSAMPLE=1000000;
    if (threader->window->getAcquire() && !(nIter%NSAMPLE)) {
      double normIter=nIter/NSAMPLE;
      threader->window->setIterations(nIter/NSAMPLE);

      threader->uProf->addPoint(normIter, E);
      threader->tProf->addPoint(normIter, T);
      threader->uVsTProf->addPoint(T, E); 

      threader->mProf->addPoint(normIter,M);
      threader->mVsTProf->addPoint(T, M); 
      
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

  PlotProfile uProf,tProf,uVsTProf, mProf, mVsTProf;

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

  // U and T time series
  {
    PRectF rect;
    rect.setXmin(0);
    rect.setXmax(1000);
    rect.setYmin(-4.0);
    rect.setYmax(+4.0);
    PlotView *view = window.getUSeriesView();
    view->add(&uProf);
    view->add(&tProf);
    view->setRect(rect);
  }

  // U vs T series
  {
    PRectF rect;
    rect.setXmin(0);
    rect.setXmax(4);
    rect.setYmin(-4.0);
    rect.setYmax(0.0);
    PlotView *view = window.getUVsTView();
    view->add(&uVsTProf);
    view->setRect(rect);
  }

  // M time series
  {
    PRectF rect;
    rect.setXmin(0);
    rect.setXmax(1000);
    rect.setYmin(-4.0);
    rect.setYmax(+4.0);
    PlotView *view = window.getMSeriesView();
    view->add(&mProf);
    view->setRect(rect);
  }

  // M vs T series
  {
    PRectF rect;
    rect.setXmin(0);
    rect.setXmax(4);
    rect.setYmin(-4.0);
    rect.setYmax(4.0);
    PlotView *view = window.getMVsTView();
    view->add(&mVsTProf);
    view->setRect(rect);
  }

  Threader threader;
  threader.window=&window;
  threader.model= &model;
  threader.uProf = &uProf;
  threader.tProf = &tProf;
  threader.uVsTProf=&uVsTProf;
  threader.mProf = &mProf;
  threader.mVsTProf=&mVsTProf;

  std::thread evolveThread (evolve,&threader); 

 

  QTimer *timer=new QTimer;
  timer->setInterval(100); // milliseconds
  
  
  QObject::connect(timer, SIGNAL(timeout()), &window, SLOT(updatePixmap()));
  timer->start();

  window.show();
  app.exec();
  finish=true;
  evolveThread.join();
  return 1;
}


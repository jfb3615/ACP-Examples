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
#include <QThread>
#include <QToolBar>
#include <QTimer>
#include <QAction>
#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>
#include <memory>
#include "Threader.h"

class SignalCatcher:public QObject {

  Q_OBJECT

public:

  SignalCatcher(Threader * t):t(t){}

public slots:

  void acquire() {
    t->acquire();
  }
  
  void reset() {
    t->reset();
  }

  void updateHistogramPlots(){
    IsingModelWidget *window=t->getWindow();
    PlotView *u=window->getUHistView();
    PlotView *m=window->getMHistView();
    PlotStream uStream(u->titleTextEdit());
    PlotStream mStream(m->titleTextEdit());
    uStream << PlotStream::Clear() << "Mean " << t->getMeanHistE() << " variance " << t->getVarianceHistE() << PlotStream::EndP();
    mStream << PlotStream::Clear() << "Mean " << t->getMeanHistM() << " variance " << t->getVarianceHistM() << PlotStream::EndP();
    u->recreate();
    m->recreate();
  }

  void record() {
    t->record();
  }

  void mkCorr() {
    t->mkCorr();
  }
  
private:

  Threader *t;
};


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
  IsingModel       model(N,N,3.0);
  IsingModelWidget window(&model);

  QThread thread;
  Threader threader(&model,&window);
  threader.moveToThread(&thread);

  SignalCatcher signalCatcher(&threader);
  
  QObject::connect(&window,&IsingModelWidget::signalReset,  &signalCatcher, &SignalCatcher::reset);
  QObject::connect(&window,&IsingModelWidget::signalAcquire,&signalCatcher, &SignalCatcher::acquire);
  QObject::connect(&window,&IsingModelWidget::signalRecord,&signalCatcher, &SignalCatcher::record);
  QObject::connect(&window,&IsingModelWidget::signalMkCorr,&signalCatcher, &SignalCatcher::mkCorr);


  
  QTimer *timer=new QTimer;
  timer->setSingleShot(true);
  QObject::connect(timer,&QTimer::timeout, &threader,&Threader::evolve);
  thread.start();
  timer->start();

  QTimer *twoTimer=new QTimer;
  twoTimer->setInterval(100); // milliseconds

  QTimer *threeTimer=new QTimer;
  threeTimer->setInterval(2000); // milliseconds

  
  QObject::connect(twoTimer, SIGNAL(timeout()), &window, SLOT(updatePixmap()));
  QObject::connect(threeTimer, SIGNAL(timeout()), &signalCatcher, SLOT(updateHistogramPlots()));
  twoTimer->start();
  threeTimer->start();
  
  window.show();
  app.exec();
  
  thread.requestInterruption();
  thread.quit();
  thread.wait();
  
  return 0;
}
#include "ising2D.moc"



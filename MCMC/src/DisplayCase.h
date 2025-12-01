#ifndef _DISPLAYCASE_H_
#define _DISPLAYCASE_H_
#include "Simulator.h"
#include "Autocorr.h"
#include "QatPlotWidgets/PlotView.h"
#include <QObject>
#include <condition_variable> 
#include <mutex> 

class DisplayCase: public QObject {
  Q_OBJECT

public:

  DisplayCase(unsigned int duration);
  void add (Simulator *simulator) {
    acceptanceRateView.add(&simulator->getAcceptanceRateProf());
    autocorr.push_back(&simulator->getAutocorrs());
    cv.push_back(&simulator->getConditionVariable());
    mtx.push_back(&simulator->getMutex());
    ready.push_back(&simulator->getReady());
    processed.push_back(&simulator->getProcessed());
    aRate.push_back(&simulator->getAcceptanceRate());
  }

  PlotView *getAcceptanceRateView() { return &acceptanceRateView;}
  PlotView *getAutocorrView() { return &autocorrView;}

public slots:
  void update();

private:

  unsigned int duration{1000};
  PlotView acceptanceRateView{PRectF{0.0,1000,0.0, 1.2}};
  PlotView autocorrView{PRectF{0.0,1000,-1.2, 1.2}};
  std::vector<const std::vector<Autocorr> *>   autocorr;
  std::vector<const std::vector<double> *>     aRate;
  std::vector<std::condition_variable *> cv;
  std::vector<std::mutex *>              mtx;
  std::vector<bool *>                    ready;
  std::vector<bool *>                    processed;
  PlotProfile                            *autocorrProf{nullptr};
  PlotProfile                             aRateProf;
  
};
#endif

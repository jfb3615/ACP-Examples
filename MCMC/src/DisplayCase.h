#ifndef _DISPLAYCASE_H_
#define _DISPLAYCASE_H_
#include "Simulator.h"
#include "Autocorr.h"
#include "QatPlotWidgets/PlotView.h"
#include "QatPlotting/PlotProfile.h"
#include "QatPlotting/PlotHist1D.h"
#include <QObject>
#include <condition_variable> 
#include <mutex>
#include <memory>

class DisplayCase: public QObject {
  Q_OBJECT

public:

  DisplayCase(unsigned int duration);
  void add (Simulator *simulator) {
    autocorr.push_back(&simulator->getAutocorrs());
    cv.push_back(&simulator->getConditionVariable());
    mtx.push_back(&simulator->getMutex());
    ready.push_back(&simulator->getReady());
    processed.push_back(&simulator->getProcessed());
    aRate.push_back(&simulator->getAcceptanceRate());
    histogram.push_back(&simulator->getPairCorrelationHistogram());
  }

  PlotView *getAcceptanceRateView() { return &acceptanceRateView;}
  PlotView *getAutocorrView() { return &autocorrView;}
  PlotView *getPairCorrView() { return &pairCorrView;}

public slots:
  void update();

private:

  unsigned int duration{1000};
  PlotView acceptanceRateView{PRectF{0.0,1000,0.0, 1.2}};
  PlotView autocorrView{PRectF{0.0,1000,-1.2, 1.2}};
  PlotView pairCorrView{PRectF{0.0, 0.5, -1.2, 1.2}};
  std::vector<const std::vector<Autocorr> *>   autocorr;
  std::vector<const std::vector<double> *>     aRate;
  std::vector<const Hist1D *>                  histogram;
  Hist1D                                       sumHistogram{"Combined",200, 0, 0.5};
  
  std::vector<std::condition_variable *> cv;
  std::vector<std::mutex *>              mtx;
  std::vector<bool *>                    ready;
  std::vector<bool *>                    processed;
  PlotProfile                            *autocorrProf{nullptr};
  PlotProfile                             aRateProf;
  std::unique_ptr<PlotHist1D>            pairCorrPlot;
  
};
#endif

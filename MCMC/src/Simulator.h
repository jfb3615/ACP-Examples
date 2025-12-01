#ifndef _SIMULATOR_H_
#define _SIMULATOR_H_
#include "Autocorr.h"
#include "QatDataAnalysis/Hist1D.h"
#include <vector>
#include <QObject>
#include <memory>
#include <mutex>
#include <condition_variable>
class AbsModel;
class Simulator: public QObject {
  
  Q_OBJECT

public:
  
  Simulator(AbsModel *model,double stepSize, unsigned int duration, unsigned int burnin);
  

  const std::vector<Autocorr> &  getAutocorrs()  const {return autocorrs;};
  const std::vector<double>   &  getAcceptanceRate() const {return aRate;}
  const Hist1D                &  getPairCorrelationHistogram() const {return pairCorrHist;}
  double       getStepSize()      const {return stepSize;}
  std::condition_variable &   getConditionVariable() {return cv;}
  std::mutex              &   getMutex()             {return mtx;}
  bool                    &   getReady()             {return ready;}
  bool                    &   getProcessed()         {return processed;}

  static constexpr unsigned int         getNAUTO()       {return NAUTO;}
  static constexpr unsigned int         getSAMPLEFREQ()  {return SAMPLEFREQ;}
  
private:
  constexpr static double          L{1.0};
  constexpr static unsigned int    NAUTO{100};
  constexpr static unsigned int    SAMPLEFREQ{1000};
  
  unsigned int                     k{0};
  AbsModel                        *model{nullptr};
  const double                     stepSize{1.0};
  const unsigned int               duration{1000};
  const unsigned int               burnin;
  std::vector<Autocorr>            autocorrs;
  std::vector<double>              aRate;          
  Hist1D                           pairCorrHist;

  // The following variables are for communication synchronization
  std::condition_variable          cv{};
  std::mutex                       mtx{};
  bool                             ready{false};
  bool                             processed{false};

					       
public slots:
  
  void simulate();
  
};
#endif

#ifndef _Threader_h_
#define _Threader_h_

#include "QatPlotting/PlotProfile.h"
#include "QatDataAnalysis/Hist1D.h"
#include "QatPlotting/PlotHist1D.h"
#include <QObject>
#include <mutex>
#include <memory>

class IsingModel;
class IsingModelWidget;

class Threader:public QObject  {

 Q_OBJECT
  
public:

  Threader(IsingModel *model,IsingModelWidget *window);

  void evolve();
  double getMeanHistE(); 
  double getMeanHistM();
  double getVarianceHistE(); 
  double getVarianceHistM();
  void record();
  void mkCorr();

  IsingModelWidget *getWindow() {return window;};
	       
  public slots:
    
    void reset();
    void acquire();
  
 private:

  void computeCorr(Hist1D * h1,
		   Hist1D * h2,
		   Hist1D * h12,
		   Hist1D * h1h2,
		   Hist1D * n,
		   unsigned int start);
  
  // These are released and reallocated. 
  std::unique_ptr<PlotProfile>      uProf;
  std::unique_ptr<PlotProfile>      tProf;
  std::unique_ptr<PlotProfile>      mProf;

  // These persist for the lifetime of the program. 
  PlotProfile      uVsTProf;
  PlotProfile      mVsTProf;
  PlotProfile      cVsTProf;
  PlotProfile      xVsTProf;

  Hist1D energyHist       {"Energy", 4000, -4.0, 4.0};
  Hist1D magnetizationHist{"Magnetization", 4000, -4.0, 4.0};
  Hist1D normalizedEnergyHist{energyHist};
  Hist1D normalizedMagnetizationHist{magnetizationHist};
  Hist1D pairCorrHist     {"PairCorrelation",200, 0.0, 200}; 
  Hist1D normalizedPairCorrHist{pairCorrHist}; 
  PlotHist1D energyPlot{normalizedEnergyHist};
  PlotHist1D magnetizationPlot{normalizedMagnetizationHist};
  PlotHist1D pairCorrPlot{normalizedPairCorrHist};
  

  
  IsingModel       *model{nullptr};
  IsingModelWidget *window{nullptr};
  PlotProfile::Properties red;
  PlotProfile::Properties blue;
  bool acquiring{false};
  std::mutex mtx{};
  std::mutex mgMtx{};
};
#endif




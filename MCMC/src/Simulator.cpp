#include "Simulator.h"
#include "AbsModel.h"
#include <QThread>
#include <iostream>
Simulator::Simulator(AbsModel *model,double stepSize, unsigned int duration, unsigned int burnin):
  model(model),stepSize(stepSize),duration(duration),burnin(burnin), pairCorrHist("PairCorr", 200, 0.0, 0.5) {
  for (unsigned int i=0;i<NAUTO;i++) {
    autocorrs.emplace_back(duration/NAUTO*(i+1));
    for (unsigned int p=0;p<model->getNumParticles();p++) {
      const double *x=&model->getPosition(p).x();
      const double *y=&model->getPosition(p).y();
      const double *z=&model->getPosition(p).z();
      autocorrs.back().followVariable(x);
      autocorrs.back().followVariable(y);
      autocorrs.back().followVariable(z);
    }
  }
}

void Simulator::simulate()
{
  using namespace Eigen;
  //
  // Call time evolution to update position array x[]:
  //
  unsigned int successes{0},totals{0};
  while (!thread()->isInterruptionRequested()) {
    bool accept=model->takeStep(stepSize);
    if (accept) successes++;
    totals++;

    // Autocorrelation:
    for (Autocorr & a: autocorrs) a.addDataPoint(k);

    // Pair correlation function
    // This time consuming calculation is performed only after burnin. 
    if (k>burnin) {
      for (unsigned int i=0;i<model->getNumParticles();i++) {
	for (unsigned int j=i+1;j<model->getNumParticles();j++) {
	  const VectorXd & x1=model->getPosition(i);
	  const VectorXd & x2=model->getPosition(j);
	  Vector3d x=x1-x2;
	  // Apply the minimum image condition:
	  for (unsigned int d=0;d<3;d++) {
	    while (x[d] > L/2.0)  x[d]-=L;
	    while (x[d] < -L/2.0) x[d]+=L;
	  }
	  double   r=x.norm();
	  pairCorrHist.accumulate(r,1/r/r);
	}
      }
    }

    
    if (!(k++ % SAMPLEFREQ)) { // 1000, unless I changed it. 



      // wait until DisplayWindown sends request
      std::unique_lock lk(mtx);
      cv.wait(lk, [this]{ return ready; });
       


      double acceptRate=double(successes)/double(totals);
      aRate.push_back(acceptRate);
      
      successes=totals=0;

      for (Autocorr & a: autocorrs) a.compute();
    

   
      
      // send data back to main()
      processed = true;
 
      // manual unlocking is done before notifying, to avoid waking up
      // the waiting thread only to block again (see notify_one for details)
      lk.unlock();
      cv.notify_one();

    }
  }
  return;
    
}

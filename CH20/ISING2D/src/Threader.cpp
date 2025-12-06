#include "Threader.h"
#include "IsingModel.h"
#include "IsingModelWidget.h"
#include <QThread>
#include <iostream>
#include <random>
#include <thread>
void Threader::computeCorr(Hist1D * h1,
			   Hist1D * h2,
			   Hist1D * h12,
			   Hist1D * h1h2,
			   Hist1D * n,
			   unsigned int start) {
  Hist1D h1Local  =*h1;
  Hist1D h2Local  =*h2;
  Hist1D h12Local =*h12;
  Hist1D h1h2Local=*h1h2;
  Hist1D nLocal   =*n;

  {
    int NX=model->NX(), NY=model->NY(); 

    for ( int i=start;i<NX;i+=16) { // Do only a slice
      for ( int iP=i+1;iP<NX ;iP++) {
	int dx = std::abs(iP-i); if (dx>200) dx-=400;
	for ( int j=0;j<NY;j++) {
	  for ( int jP=j+1;jP<NY;jP++) {
	    {
	      int dy = std::abs(jP-j); if (dy>200) dy-=400;
	      double r= std::sqrt( (double) (dx*dx+dy*dy));
	      double s1=model->isUp(i,j)? 1.0 : 0.0 , s2=model->isUp(iP,jP)? 1.0 : 0.0;
	      h1Local.accumulate(r,s1);
	      h2Local.accumulate(r,s2);
	      h12Local.accumulate(r,s1); // idempotent
	      h1h2Local.accumulate(r,s1*s2);
	      nLocal.accumulate(r);
	    }
	  }
	}
      }
    }
  }
  {
    std::unique_lock<std::mutex> lock(mgMtx);
    (*h1)+=h1Local;
    (*h2)+=h2Local;
    (*h12)+=h12Local;
    (*h1h2)+=h1h2Local;
    (*n)+=nLocal;
  }
}

Threader::Threader(IsingModel *model,IsingModelWidget *window):model(model),window(window)
{
  // Set some plot properties:
  {
    for (PlotProfile::Properties * p : {&red,&blue}) p->brush.setStyle(Qt::SolidPattern);
    red.brush.setColor("darkRed");
    red.pen.setColor("darkRed");
    blue.brush.setColor("darkBlue");
    blue.pen.setColor("darkBlue");
    
    mVsTProf.setProperties(blue);
    uVsTProf.setProperties(blue);
    cVsTProf.setProperties(blue);
    xVsTProf.setProperties(blue);

  }

  {
    PlotHist1D::Properties prop;
    prop.pen.setWidth(2);
    prop.pen.setColor("darkBlue");
    for (PlotHist1D * p: {&energyPlot, &magnetizationPlot,&pairCorrPlot} ) p->setProperties(prop);
    
  }
  


  window->getUVsTView()->add(&uVsTProf);
  window->getMVsTView()->add(&mVsTProf);
  window->getCVsTView()->add(&cVsTProf);
  window->getXVsTView()->add(&xVsTProf);
  window->getUHistView()->add(&energyPlot);
  window->getMHistView()->add(&magnetizationPlot);
  window->getCorrelationFcnView()->add(&pairCorrPlot);

}


void Threader::evolve() {
	       
  unsigned long int nIter=0;
  while (!thread()->isInterruptionRequested()) {
    model->next();
    unsigned int i=0, j=0;
    int delta=0;
    
    model->lastStep(i,j,delta);
    bool on=model->isUp(i,j);
    window->updateImage(i,j,on);
    nIter++;

    const double norm=model->NX()*model->NY();
    double E=model->U()/norm;
    double T=model->tau();
    double M=model->M()/norm;
    const unsigned int NSAMPLE=1000000;
    
    if (acquiring) {
      {
	std::unique_lock<std::mutex> lock(mtx);
	energyHist.accumulate(E);
	magnetizationHist.accumulate(M);
      }
      if (!(nIter%NSAMPLE)) {
	
	double normIter=nIter/NSAMPLE;
	window->setIterations(nIter/NSAMPLE);
	
	uProf->addPoint(normIter, E);
	tProf->addPoint(normIter, T);
	
	mProf->addPoint(normIter,M);

	normalizedEnergyHist.clear();
	normalizedEnergyHist+=energyHist;
	normalizedEnergyHist*=(1/normalizedEnergyHist.sum()/normalizedEnergyHist.binWidth());

	normalizedMagnetizationHist.clear();
	normalizedMagnetizationHist+=magnetizationHist;
	normalizedMagnetizationHist*=(1/normalizedMagnetizationHist.sum()/normalizedMagnetizationHist.binWidth());

      }
    }
 }
}


// Clear several of the views and delete the PlotProfiles
void Threader::reset() {
  acquiring=false;
  
  if (uProf) uProf.release();
  if (tProf) tProf.release();
  window->getUSeriesView()->clear();


  if (mProf) mProf.release();
  window->getMSeriesView()->clear();

  energyHist.clear();
  magnetizationHist.clear();
  pairCorrHist.clear();

  
}

// Create the Plot Profiles set their properties and add them
// to the view.

void Threader::acquire() {
  uProf=std::make_unique<PlotProfile>();
  uProf->setProperties(blue);
  window->getUSeriesView()->add(uProf.get());

  tProf=std::make_unique<PlotProfile>();
  window->getUSeriesView()->add(tProf.get());
  tProf->setProperties(red);

  mProf=std::make_unique<PlotProfile>();
  window->getMSeriesView()->add(mProf.get());
  mProf->setProperties(blue);
  acquiring=true;
}

double Threader::getMeanHistE() {
  std::unique_lock<std::mutex> lock(mtx);
  return energyHist.mean();
}
double Threader::getMeanHistM() {
  std::unique_lock<std::mutex> lock(mtx);
  return magnetizationHist.mean();
}


double Threader::getVarianceHistE() {
  std::unique_lock<std::mutex> lock(mtx);
  return energyHist.variance();
}
double Threader::getVarianceHistM() {
  std::unique_lock<std::mutex> lock(mtx);
  return magnetizationHist.variance();
}

void Threader::record() {
  if (acquiring) {
    std::unique_lock<std::mutex> lock(mtx);
    double T=model->tau();
    mVsTProf.addPoint(T, magnetizationHist.mean());
    uVsTProf.addPoint(T, energyHist.mean());
    xVsTProf.addPoint(T, magnetizationHist.variance()/T/T);
    cVsTProf.addPoint(T, energyHist.variance()/T);

 }
}

void Threader::mkCorr() {
 
  {
    std::unique_lock<std::mutex> lock(mtx);




    pairCorrHist.clear();


    Hist1D h1=pairCorrHist,h2=pairCorrHist, h12=pairCorrHist,h1h2=pairCorrHist,n=pairCorrHist;

    unsigned int NTHREADS=16;

    std::vector<std::thread> threadVec;
    for (unsigned int iThread=0;iThread<NTHREADS;iThread++) {
      threadVec.emplace_back(std::thread(&Threader::computeCorr, this,&h1,&h2,&h12,&h1h2,&n,iThread));
    }
    
    for (unsigned int iThread=0;iThread<NTHREADS;iThread++) {
      threadVec[iThread].join();
    }

   
    normalizedPairCorrHist.clear();
    for (unsigned int b=1;b<pairCorrHist.nBins();b++) {
      double v1=h1.bin(b);
      double v2=h2.bin(b);
      double v12=h12.bin(b);
      double v1v2=h1h2.bin(b);
      double vn=n.bin(b);

      normalizedPairCorrHist.accumulate(n.binCenter(b),(v1v2-v1*v2/vn)/(v12-v1*v1/vn));
    }
    std::cout << "\a" << std::flush;
  }
  
}

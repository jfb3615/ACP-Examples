#include "DisplayCase.h"
#include "QatPlotting/PlotStream.h"
#include <iostream>

void DisplayCase::update() {
  acceptanceRateView.recreate();

  // We signal that we are ready for data from the simulators
  for (size_t i=0;i<ready.size();i++)
  {
    std::lock_guard lk(*mtx[i]);
    (*ready[i]) = true;
  } 

  // Se notify the simulators.
  for (size_t i=0;i<cv.size();i++) {
    cv[i]->notify_one();
  }

  // We wait until the simulators are done.
  for (size_t i=0;i<cv.size();i++) {
    std::unique_lock lk(*mtx[i]);
    cv[i]->wait(lk, [this,i]{return *processed[i]; });
  }
  // Update the autocorr plots:

  autocorrView.clear();
  delete autocorrProf;
  autocorrProf=new PlotProfile;
  {
    PlotProfile::Properties prop;
    prop.symbolSize=10;
    prop.pen.setStyle(Qt::NoPen);
    prop.brush.setStyle(Qt::SolidPattern);
    prop.brush.setColor("darkBlue");
    autocorrProf->setProperties(prop);
    aRateProf.setProperties(prop);
  }
  
  std::vector<double> aveAuto(autocorr[0]->size());
  for (unsigned int i=0;i<autocorr.size();i++) {
    for (unsigned int t=0;t<autocorr[0]->size();t++) aveAuto[t]+= (*autocorr[i])[t].harvest();
  }
  for (unsigned int t=0;t<aveAuto.size();t++) {
    aveAuto[t]/=autocorr.size();
    if (std::isfinite(aveAuto[t])) autocorrProf->addPoint(duration/Simulator::getNAUTO()*double(t), aveAuto[t]);
  }

  autocorrView.add(autocorrProf);
  autocorrView.recreate();


  double aveAccRate{0};
  for (unsigned int i=0;i<aRate.size();i++) aveAccRate+=aRate[i]->back();
  aveAccRate/=aRate.size();
  aRateProf.addPoint(aRate[0]->size(),aveAccRate);
  acceptanceRateView.recreate();
}

DisplayCase::DisplayCase(unsigned int duration):
  duration(duration),
  autocorrView({0.0,(double) duration,-1.2, 1.2}){
  acceptanceRateView.add(&aRateProf);
  std::map<PlotView *, std::string> yLabel={{&acceptanceRateView, "Acceptance Rate"}, {&autocorrView, "Autocorrelation"}};
  for (PlotView * v: {&acceptanceRateView,&autocorrView} ) {
    PlotStream xLabelStream(v->xLabelTextEdit());
    xLabelStream << PlotStream::Clear()
		 << PlotStream::Center()
		 << PlotStream::Family("Arial")
		 << PlotStream::Size(16)
		 << "step/" << (int) Simulator::getSAMPLEFREQ()
		 << PlotStream::EndP();
    
    PlotStream yLabelStream(v->yLabelTextEdit());
    yLabelStream << PlotStream::Clear()
		 << PlotStream::Center()
		 << PlotStream::Family("Arial")
		 << PlotStream::Size(16)
		 << yLabel[v]
		 << PlotStream::EndP();
  }
}


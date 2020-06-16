#include "Compton.h"
#include "KleinNishina.h"
#include "QatPlotWidgets/PlotView.h"
#include "QatPlotWidgets/MultipleViewWindow.h"
#include "QatPlotting/RealArg.h"
#include "QatPlotting/PlotStream.h"
#include "QatPlotting/PlotFunction1D.h"
#include "QatInventorWidgets/PolarFunctionView.h"
#include "QatGenericFunctions/F1D.h"
#include "QatGenericFunctions/F2D.h"
#include "QatGenericFunctions/CutBase.h"
#include "QatGenericFunctions/GaussIntegrator.h"
#include "QatGenericFunctions/GaussQuadratureRule.h"
#include <Inventor/Qt/SoQt.h>
#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <cstdlib>
#include <iostream>
#include <string>
#include <complex>
#include <functional>
#include <ratio>

bool pol=false;
bool fast=false;

double kG=0.0;
double ssigma(double cosTheta) {
  if (fast) {
    KleinNishina compton(kG,pol);
    return compton.sigma(cosTheta);
  }
  else {
    Compton compton(kG,pol);
    return compton.sigma(cosTheta);
  }
}

template <class T>
double rsigma (double cosTheta) {
  static const double m=0.511;
  static double k=T::num/double(T::den)*m;
  if (fast) {
    static KleinNishina compton(k,pol);
    return compton.sigma(cosTheta);
  }
  else {
    static Compton compton(k,pol);
    return compton.sigma(cosTheta);
  }
}

template <class T>
double rsigma2 (double cosTheta,double phi) {
  static const double m=0.511;
  static double k=T::num/double(T::den)*m;
  if (fast) {
    static KleinNishina compton(k,pol);
    return log(compton.sigma(cosTheta,phi));
  }
  else {
    static Compton compton(k,pol);
    return log(compton.sigma(cosTheta,phi));
  }
}


template <int NPOINTS>
double totalCrossSection(double k) {
  kG=k;
  Genfun::GaussLegendreRule glRule(NPOINTS);
  Genfun::GaussIntegrator integrator(glRule);
  Genfun::F1D Sigma=ssigma;
  return 2*M_PI*integrator(Sigma);
}


int main (int argc, char * * argv) {

  // Automatically generated:-------------------------:

  {
    std::string usage= std::string("usage: ") + argv[0] + "[-p] [-fast] "; 
    for (int i=1;i<argc;i++) {
      if (argv[i]==std::string("-p")) {
	pol=true;
      }
      else if (argv[i]==std::string("-fast")) {
	fast=true;
      }
      else {
	std::cerr << usage << std::endl;
	exit(0);
      }
    }
    
    
    
    
    
    QApplication     app(argc,argv);
    
    MultipleViewWindow window;
    SoQt::init(&window);
    
    QToolBar *toolBar=window.addToolBar("Tools");
    QAction  *quitAction=toolBar->addAction("quit");
    
    quitAction->setShortcut(QKeySequence("q"));
    
    QObject::connect(quitAction, SIGNAL(triggered()), &app, SLOT(quit()));
    
    PRectF rect;
    rect.setXmin(0.001);
    rect.setXmax(100000);
    rect.setYmin(0.01);
    rect.setYmax(50);
    
    
    PlotView sigTotView(rect);
    sigTotView.setLogX(true);
    sigTotView.setLogY(true);
    window.add(&sigTotView, "σ");
    
    PolarFunctionView polarFunctionViewHundredth(&window);
    window.add(&polarFunctionViewHundredth,"polar k=m/100");
    
    PolarFunctionView polarFunctionViewTenth(&window);
    window.add(&polarFunctionViewTenth,"polar k=m/10");
    
    PolarFunctionView polarFunctionViewHalf(&window);
    window.add(&polarFunctionViewHalf,"polar k=m/2");
    
    PolarFunctionView polarFunctionViewUnit(&window);
    window.add(&polarFunctionViewUnit,"polar k=m");
    
    PolarFunctionView polarFunctionViewTwice(&window);
    window.add(&polarFunctionViewTwice,"polar k=2m");
    
    PolarFunctionView polarFunctionViewTenTimes(&window);
    window.add(&polarFunctionViewTenTimes,"polar k=10m");
    
    Genfun::F1D compton=totalCrossSection<8>;
    PlotFunction1D pCompton=compton;
    {
      PlotFunction1D::Properties prop;
      prop.pen.setWidth(3);
      prop.pen.setColor("darkRed");
      pCompton.setProperties(prop);
    }
    sigTotView.add(&pCompton);
    
    {
      PlotStream titleStream(sigTotView.titleTextEdit());
      titleStream << PlotStream::Clear()
		  << PlotStream::Center() 
		  << PlotStream::Family("Sans Serif") 
		  << PlotStream::Size(14)
		  << "Total Compton cross section in lowest order" 
		  << PlotStream::EndP();
      
      
      PlotStream xLabelStream(sigTotView.xLabelTextEdit());
      xLabelStream << PlotStream::Clear()
		   << PlotStream::Center()
		   << PlotStream::Family("Sans Serif")
		   << PlotStream::Size(16)
		   << "k"
		   << " [MeV]"
		   << PlotStream::EndP();
      
      PlotStream yLabelStream(sigTotView.yLabelTextEdit());
      yLabelStream << PlotStream::Clear()
		   << PlotStream::Center()
		   << PlotStream::Family("Sans Serif")
		   << PlotStream::Size(16)
		   << "σ [units of r"
		   << PlotStream::Sub()
		   << "e"
		   << PlotStream::Super()
		   << "2"
		   << PlotStream::Normal()
		   << "]"
		   << PlotStream::EndP();
    }
    
    rect.setXmin(-1.0);
    rect.setXmax(1.0);
    rect.setYmin(0.0);
    rect.setYmax(1.2);
    PlotView dsigdOmegaView(rect);
    window.add(&dsigdOmegaView, "dσ/dΩ");
    
    {
      PlotStream statStream(dsigdOmegaView.statTextEdit());
      statStream << PlotStream::Clear()
		 << PlotStream::Left() 
		 << PlotStream::Family("Sans Serif") 
		 << PlotStream::Size(12)
		 << PlotStream::Color("darkBlue") << "―k=m/100\n"
		 << PlotStream::Color("darkRed") << "―k=m/10\n"
		 << PlotStream::Color("darkGreen") << "―k=m/2\n"
		 << PlotStream::Color("black") << "―k=m\n"
		 << PlotStream::Color("darkCyan") << "―k=2m\n"
		 << PlotStream::Color("darkMagenta") << "―k=10m\n"
		 << PlotStream::EndP();

      PlotStream titleStream(dsigdOmegaView.titleTextEdit());
      titleStream << PlotStream::Clear()
		  << PlotStream::Center() 
		  << PlotStream::Family("Sans Serif") 
		  << PlotStream::Size(12)
		  << "Compton differential cross section in lowest order" 
		  << PlotStream::EndP();
      
      
      PlotStream xLabelStream(dsigdOmegaView.xLabelTextEdit());
      xLabelStream << PlotStream::Clear()
		   << PlotStream::Center()
		   << PlotStream::Family("Sans Serif")
		   << PlotStream::Size(16)
		   << "cos (θ)"
		   << PlotStream::EndP();
      
      PlotStream yLabelStream(dsigdOmegaView.yLabelTextEdit());
      yLabelStream << PlotStream::Clear()
		   << PlotStream::Center()
		   << PlotStream::Family("Sans Serif")
		   << PlotStream::Size(16)
		   << "dσ/dΩ [units of r"
		   << PlotStream::Sub()
		   << "e"
		   << PlotStream::Super()
		   << "2"
		   << PlotStream::Normal()
		   << "]"
		   << PlotStream::EndP();
    }
    
    Genfun::F1D sigmakEqMBy100=rsigma<std::ratio<1,100>>;
    Genfun::F1D sigmakEqMBy10=rsigma<std::ratio<1,10>>;
    Genfun::F1D sigmakEqMBy2=rsigma<std::ratio<1,2>>;
    Genfun::F1D sigmakEqM=rsigma<std::ratio<1,1>>;
    Genfun::F1D sigmakEq2M=rsigma<std::ratio<2,1>>;
    Genfun::F1D sigmakEq10M=rsigma<std::ratio<10,1>>;
    const Cut<double> & cut = RealArg::Gt(-0.99) && RealArg::Lt(1.0);
    PlotFunction1D pHundredth(sigmakEqMBy100,cut);
    PlotFunction1D pTenth(sigmakEqMBy10,cut);
    PlotFunction1D pHalf(sigmakEqMBy2,cut);
    PlotFunction1D pUnit(sigmakEqM,cut);
    PlotFunction1D pTwice(sigmakEq2M,cut);
    PlotFunction1D pTenTimes(sigmakEq10M,cut);

    {
      PlotFunction1D::Properties prop;
      prop.pen.setWidth(3);
      prop.pen.setColor("darkBlue");
      pHundredth.setProperties(prop);
      prop.pen.setColor("darkRed");
      pTenth.setProperties(prop);
      prop.pen.setColor("darkGreen");
      pHalf.setProperties(prop);
      prop.pen.setColor("black");
      pUnit.setProperties(prop);
      prop.pen.setColor("darkCyan");
      pTwice.setProperties(prop);
      prop.pen.setColor("darkMagenta");
      pTenTimes.setProperties(prop);
    }      
    
    dsigdOmegaView.add(&pHundredth);
    dsigdOmegaView.add(&pTenth);
    dsigdOmegaView.add(&pHalf);
    dsigdOmegaView.add(&pUnit);
    dsigdOmegaView.add(&pTwice);
    dsigdOmegaView.add(&pTenTimes);
    
    Genfun::F2D polarkEqMBy100=rsigma2<std::ratio<1,100>>;
    Genfun::F2D polarkEqMBy10=rsigma2<std::ratio<1,10>>;
    Genfun::F2D polarkEqMBy2=rsigma2<std::ratio<1,2>>;
    Genfun::F2D polarkEqM=rsigma2<std::ratio<1,1>>;
    Genfun::F2D polarkEq2M=rsigma2<std::ratio<2,1>>;
    Genfun::F2D polarkEq10M=rsigma2<std::ratio<10,1>>;
    
    polarFunctionViewHundredth.setFunction(&polarkEqMBy100);
    polarFunctionViewTenth.setFunction(&polarkEqMBy10);
    polarFunctionViewHalf.setFunction(&polarkEqMBy2);
    polarFunctionViewUnit.setFunction(&polarkEqM);
    polarFunctionViewTwice.setFunction(&polarkEq2M);
    polarFunctionViewTenTimes.setFunction(&polarkEq10M);
    
    
    window.show();
    SoQt::mainLoop();
  }
  SoQt::done();
  
  return 1;
}


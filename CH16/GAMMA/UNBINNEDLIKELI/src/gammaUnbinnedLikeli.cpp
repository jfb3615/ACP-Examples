#include "QatPlotting/PlotStream.h"
#include "QatPlotWidgets/PlotView.h"
#include "QatDataAnalysis/Hist1D.h"
#include "QatDataAnalysis/Hist1DMaker.h"
#include "QatDataAnalysis/Table.h"
#include "QatPlotting/PlotHist1D.h"
#include "QatPlotting/PlotProfile.h"
#include "QatPlotting/PlotFunction1D.h"
#include "QatPlotting/PlotErrorEllipse.h"
#include "QatPlotting/PlotPoint.h"
#include "QatDataModeling/HistChi2Functional.h"
#include "QatDataModeling/HistLikelihoodFunctional.h"
#include "QatDataModeling/TableLikelihoodFunctional.h"
#include "QatDataModeling/MinuitMinimizer.h"
#include "QatGenericFunctions/Variable.h"
#include "QatGenericFunctions/Log.h"
#include "QatGenericFunctions/Exp.h"
#include "QatGenericFunctions/Gamma.h"
#include "QatGenericFunctions/IncompleteGamma.h"
#include "Eigen/Dense"
#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <QtGui>
#include <iostream>
#include <string>
#include <random>
using namespace Genfun;
int main(int argc, char ** argv ) {
  bool silent=false;
  if (argc>1 && argv[1]==std::string("-s")) silent=true;

  // SOME OVERHEAD TO GETTING THIS TO RUN IN A GUI:
  QApplication app(argc, argv);
  QMainWindow window;
  QToolBar *toolBar=window.addToolBar("Tools");
  QAction  *nextAction=toolBar->addAction("Next");
  nextAction->setShortcut(QKeySequence("n"));
  QObject::connect(nextAction, SIGNAL(triggered()), &app, SLOT(quit()));
  // INPUT VALUES TO ALPHA AND BETA
  double alphaTrue=2.0, betaTrue=2.0;
  
  // CONSTRUCT RANDOM NUMBER GENERATORS:
  unsigned int seed=999;
  std::mt19937 engine(seed);
  std::gamma_distribution<double> g(alphaTrue);


  // MAKE HISTOGRAMS TO MONITOR FIT QUALITY AND FIT ACCURACY
  Hist1D fitAccuracy("Fit Accuracy", 100, 0, 1);
  Hist1D fitQuality ("Fit Quality",  100, 0, 1);

  // D0 SOME PSEUDOEXPERIMENTS:
  const unsigned int NPSEUDO=100;
  for (unsigned int i=0; i<NPSEUDO; i++) {

    // NUMBER OF EVENTS/EXPERIMENT
    const int NTOT=10000;
    
    // MAKE A TABLE AND COLLECT THE DATA:
    Table table("Data collection");
    for (int i=0;i<NTOT;i++) {
      table.add("X",betaTrue*g(engine));
      table.capture();
    }
    
    // PREPARE THE FIT.  THIS IS A TWO-PARAMETER FIT.
    Parameter pAlpha("Alpha", 0.5, 0.1, 3.5);
    Parameter pBeta ("Beta",  0.5, 0.1, 3.5);
    Variable  X=table.symbol("X");

    // NOW MAKE A FIT FUNCTION AND CONNECT IT TO ITS PARAMETERS:
    Log log;
    Gamma gamma;
    Exp   exp;
    GENFUNCTION gammaDistribution=exp(-pAlpha*log(pBeta))/gamma(pAlpha)*exp(-X/pBeta+(pAlpha-1)*log(X));

    // MAKE A FUNCTIONAL, WHICH RETURNS THE LIKELIHOOD OF A DATA MODEL GIVEN A FIT FUNCTION
    TableLikelihoodFunctional objectiveFunction(table);
    GENFUNCTION f = gammaDistribution(X);
    
    // NOW MAKE AN ENGINE FOR MINIMIZING THE LIKELIHOOD BY VARYING THE PARAMETERS.
    bool verbose=true;
    MinuitMinimizer minimizer(verbose);
    minimizer.addParameter(&pAlpha);
    minimizer.addParameter(&pBeta);
    minimizer.addStatistic(&objectiveFunction,&f);
    minimizer.minimize();
    
    // NOW GET THE COVARIANCE MATRIX FROM THE FIT:
    Eigen::MatrixXd C(2,2);
    C(0,0)=minimizer.getError(&pAlpha,&pAlpha);
    C(1,1)=minimizer.getError(&pBeta,&pBeta);
    C(0,1)=minimizer.getError(&pAlpha,&pBeta);
    C(1,0)=minimizer.getError(&pAlpha,&pBeta);
    
    // STUFF THE DEVIATION INTO A VECTOR:
    Eigen::VectorXd V(2);
    V[0]=minimizer.getValue(&pAlpha)-alphaTrue;
    V[1]=minimizer.getValue(&pBeta) -betaTrue;
    
    // COMPUTE THE PROBILITY OF CHI^2
    int degreesOfFreedom=2;
    IncompleteGamma cumulativeChiSquareAccuracy=IncompleteGamma(IncompleteGamma::P);
    cumulativeChiSquareAccuracy.a().setValue(degreesOfFreedom/2.0);
    double chiSq=(V.transpose()*C.inverse()*V)(0,0);
    double fitAccuracyProb=1-cumulativeChiSquareAccuracy(chiSq/2.0);
    
    // MONITOR FIT ACCURACY AND QUALITY
    fitAccuracy.accumulate(fitAccuracyProb);

    // Fill a histogram:
    Hist1DMaker hMaker(X, 100, 0, 15);
    Hist1D hist = hMaker*table;

    // NOW LOOK AT THE RESULTS OF A SINGLE EXPERIMENT
    if (!silent)
    {

      // MAKE A PLOT ADAPTOR FOR THE HISTOGRAM AND SET UP SOME OF ITS PROPERTIES:
      PlotHist1D pHist(hist);
      PlotHist1D::Properties prop;
      prop.pen.setColor(QColor("darkRed"));
      prop.pen.setWidth(3);
      prop.plotStyle=PlotHist1D::Properties::SYMBOLS;
      prop.symbolSize=10;
      pHist.setProperties(prop);
    
      // MAKE A PLOT-ADAPTOR FOR THE FITTING FUNCTION:
      PlotFunction1D pGamma(NTOT*gammaDistribution*hist.binWidth());
    
      // MAKE A PLOT
      PlotView view(pHist.rectHint());
      window.setCentralWidget(&view);
      view.setBox(false);

      // ADD HISTOGRAM AND FUNCTO TO THE PLOT
      view.add(&pHist);
      view.add(&pGamma);
      
      // CONSTRUCT THREE LABEL STREAMS..
      PlotStream titleStream(view.titleTextEdit()), xLabelStream(view.xLabelTextEdit()), yLabelStream(view.yLabelTextEdit());
      
      // LABEL THE TITLE:
      titleStream
	<< PlotStream::Clear() 
	<< PlotStream::Family("Standard Symbols L")  
	<< "    G " 
	<< PlotStream::Family("Sans Serif")  
	<< PlotStream::Size(18)
	<< "Distribution (Monte Carlo)" 
	<< PlotStream::EndP();
      
      // LABEL THE X-AXIS
      yLabelStream 
	<< PlotStream::Clear() 
	<< PlotStream::Family("Standard Symbols L")
	<< PlotStream::Size(16)
	<< "\t\tr" 
	<< PlotStream::Family("Sans Serif") 
	<< "(x)" 
	<< PlotStream::EndP();
      
      // LABEL THE Y-AXIS
      xLabelStream 
	<< PlotStream::Clear() 
	<< PlotStream::Family("Sans Serif")  
	<< "\t\t\t x" 
	<< PlotStream::EndP();

      // SHOW THIS VIEW
      window.show();
      app.exec();
    
    }

    // NOW LOOK AT RESULTS COMPARED TO INPUT VALUE
    if (!silent)
    {
      // MAKE A PLOT
      double eA=sqrt(minimizer.getError(&pAlpha,&pAlpha));
      double eB=sqrt(minimizer.getError(&pBeta,&pBeta));
      PRectF rect;
      rect.setXmin(alphaTrue-4*eA);
      rect.setXmax(alphaTrue+4*eA);
      rect.setYmin(betaTrue-4*eB);
      rect.setYmax(betaTrue+4*eB);
    
      PlotView view(rect);
      window.setCentralWidget(&view);
      view.setBox(false);
      
      // SUMMARIZE THE FIT IN AN ERROR ELLIPSE:
      PlotErrorEllipse errorEllipse(minimizer.getValue(&pAlpha), 
				    minimizer.getValue(&pBeta),
				    minimizer.getError(&pAlpha,&pAlpha),
				    minimizer.getError(&pBeta, &pBeta),
				    minimizer.getError(&pAlpha,&pBeta),
				    PlotErrorEllipse::ONESIGMA);
      
      // PLOT ALSO THE TRUE VALUE OF ALPHA AND BETA:  SET SOME PROPERTIES,, TOO.
      PlotPoint trueValue(alphaTrue, betaTrue);
      {
	PlotPoint::Properties prop;
	
	prop.brush.setColor(QColor("darkRed"));
	prop.brush.setStyle(Qt::SolidPattern);
	trueValue.setProperties(prop);
      }
      
    
      // CONSTRUCT THREE LABEL STREAMS..
      PlotStream titleStream(view.titleTextEdit()), xLabelStream(view.xLabelTextEdit()), yLabelStream(view.yLabelTextEdit());
      
      // LABEL THE X-AXIS
      xLabelStream 
	<< PlotStream::Clear() 
	<< PlotStream::Family("Standard Symbols L")  
	<< "\t\t\t\ta" 
	<< PlotStream::EndP();
      
      // LABEL THE Y-AXIS
      yLabelStream 
	<< PlotStream::Clear() 
	<< PlotStream::Family("Standard Symbols L")  
	<< "\t\t b" 
	<< PlotStream::EndP();
      
      // VIEW THE ERROR ELLIPSES
      view.add(&errorEllipse);
      view.add(&trueValue);
      window.show();
      app.exec();
    }
  }

  // VIEW THE FIT ACCURACY RESULTS:
  if (1)
  {
    
    PlotHist1D pHist(fitAccuracy);
    PlotView view(pHist.rectHint());
    window.setCentralWidget(&view);
    view.setBox(false);

      // CONSTRUCT THREE LABEL STREAMS..
      PlotStream titleStream(view.titleTextEdit()), xLabelStream(view.xLabelTextEdit()), yLabelStream(view.yLabelTextEdit());
      
      // LABEL THE TITLE:
      titleStream
	<< PlotStream::Clear() 
	<< PlotStream::Size(18)
	<< "Fit Accuracy: Probability "
	<< PlotStream::Family("Standard Symbols L")  
	<< "c" 
	<< PlotStream::Super() 
	<< "2" 
	<< PlotStream::EndP();
      
      // LABEL THE X-AXIS
      xLabelStream 
	<< PlotStream::Clear() 
	<< PlotStream::Family("Standard Symbols L")  
	<< "\t\t\t\ta" 
	<< PlotStream::EndP();
      
      // LABEL THE Y-AXIS
      yLabelStream 
	<< PlotStream::Clear() 
	<< PlotStream::Family("Standard Symbols L")  
	<< "\t\t b" 
	<< PlotStream::EndP();
      
    
    
    view.add(&pHist);
    
    window.show();
    app.exec();

  }

  return 0;

}

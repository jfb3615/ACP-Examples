#include "QatDataAnalysis/OptParse.h"
#include "QatPlotting/PlotStream.h"
#include "QatPlotWidgets/PlotView.h"
#include "QatPlotWidgets/MultipleViewWindow.h"
#include "QatDataAnalysis/Hist1D.h"
#include "QatPlotting/PlotHist1D.h"
#include "QatPlotting/PlotProfile.h"
#include "QatPlotting/PlotFunction1D.h"
#include "QatPlotting/PlotErrorEllipse.h"
#include "QatPlotting/PlotPoint.h"
#include "QatPlotting/PlotKey.h"
#include "QatDataModeling/HistLikelihoodFunctional.h"
#include "QatDataModeling/MinuitMinimizer.h"
#include "QatGenericFunctions/Gamma.h"
#include "QatGenericFunctions/Exp.h"
#include "QatGenericFunctions/Log.h"
#include "QatGenericFunctions/Parameter.h"
#include "QatGenericFunctions/IncompleteGamma.h"
#include "Eigen/Dense"
#include "QatGenericFunctions/Variable.h"
#include <QMainWindow>
#include <QApplication>
#include <QToolBar>
#include <QAction>
#include <QtGui>
#include <iostream>
#include <string>
#include <random>
#include <map>
using namespace Genfun;


class SignalCatcher : public QObject {
  Q_OBJECT
public slots:
  void quit() {
    exit(0);
  }
};

#include "gammaBinnedLikeli.moc"

int main(int argc, char ** argv ) {

  //
  // Parse the command line:
  // 
  std::string usage = std::string(argv[0]) + " [NRUNS=val] [NPER=val] -s";
  NumericInput numeric;
  numeric.declare("NRUNS", "Number of runs", 100);
  numeric.declare("NPER",  "Number of events per run", 10000);
  numeric.optParse(argc, argv);
  
  if (argc>2) {
    std::cout << usage << std::endl;
    exit(0);
  }

  bool silent=false;
  if (argc>1 && argv[1]==std::string("-s")) silent=true;
  unsigned int NRUNS = (unsigned int)(0.5+numeric.getByName("NRUNS"));
  unsigned int NPER  = (unsigned int)(0.5+numeric.getByName("NPER"));

  // SOME OVERHEAD TO GETTING THIS TO RUN IN A GUI:
  QApplication app(argc, argv);
  MultipleViewWindow window;

  // ACTION: GO TO NEXT EXPERIMENT
  QToolBar *toolBar=window.addToolBar("Tools");
  QAction  *nextAction=toolBar->addAction("Next");
  nextAction->setShortcut(QKeySequence("n"));
  QObject::connect(nextAction, SIGNAL(triggered()), &app, SLOT(quit()));


  // ACTION: EXIT THE APPLICATION
  SignalCatcher signalCatcher;
  QAction  *quitAction=toolBar->addAction("Quit");
  quitAction->setShortcut(QKeySequence("q"));
  QObject::connect(quitAction, SIGNAL(triggered()), &signalCatcher, SLOT(quit()));

  // MAKE 3 PLOT VIEW (DISPLAY 2 IN "SILENT MODE);
  PlotView view1, view2, view3;
  if (!silent) window.add(&view1, "Fits");
  if (!silent) window.add(&view2, "Measurements");
  window.add(&view3, "Accuracy");

  // DEFINE PLOT LABELS:
  std::map<const PlotView *, std::string>
    xLabelMap={{&view1, "x"}, {&view2, "α"}, {&view3, "Probability χ²"}},
    yLabelMap={{&view1,  "ρ(x)"}, {&view2, "β"}, {&view3, "experiments/bin"}},
    titleMap={{&view1,  "Probability χ²="}, {&view2, "Probability χ²="}, {&view3, "fit accuracy"}};

  // FORMAT AND LABEL THE PLOTS:
  for (PlotView * view : {&view1, &view2, &view3}) {
    view->setXZero(false);
    view->setYZero(false);
    view->setGrid(false);
    view->setBox(false);
    PlotStream xLabelStream(view->xLabelTextEdit());
    xLabelStream 
	  << PlotStream::Clear()
	  << PlotStream::Center() 
	  << PlotStream::Family("Sans Serif")  
	  << PlotStream::Size(16)
	  << xLabelMap[view]
	  << PlotStream::EndP();
    PlotStream yLabelStream(view->yLabelTextEdit());
    yLabelStream 
	  << PlotStream::Clear()
	  << PlotStream::Center() 
	  << PlotStream::Family("Sans Serif")  
	  << PlotStream::Size(16)
	  << yLabelMap[view]
	  << PlotStream::EndP();
    PlotStream titleStream(view->titleTextEdit());
    titleStream 
	  << PlotStream::Clear()
	  << PlotStream::Center() 
	  << PlotStream::Family("Sans Serif")  
	  << PlotStream::Size(16)
	  << titleMap[view]
	  << PlotStream::EndP();
    
  }
  view1.yAxisFont().setPointSize(14);
  view2.yAxisFont().setPointSize(14);

  // FONT FOR PLOT LABELING
  QFont font;
  font.setPointSize(16);

  // NOW ONTO THE CALCULATIONS.... 
  
  // INPUT VALUES TO ALPHA AND BETA
  double alphaTrue=2.0, betaTrue=0.5;
  
  // CONSTRUCT RANDOM NUMBER GENERATORS:
  unsigned int seed=777;
  std::mt19937 engine(seed);
  std::gamma_distribution<double> g(alphaTrue, betaTrue);

  // MAKE HISTOGRAMS TO MONITOR FIT ACCURACY
  Hist1D fitAccuracy("Fit Accuracy", 100, 0, 1);

  // D0 SOME PSEUDOEXPERIMENTS:
  for (unsigned int i=0; i<NRUNS; i++) {

    // NUMBER OF EVENTS/EXPERIMENT
    const int NTOT=NPER;
    
    // MAKE A HISTOGRAM AND COLLECT THE DATA:
    Hist1D hist("Gamma distribution", 100, 0, 5);
    for (int i=0;i<NTOT;i++) {
      hist.accumulate(g(engine));
    }
    
    // PREPARE THE FIT.  THIS IS A TWO-PARAMETER FIT.
    Parameter pAlpha("Alpha", 0.5, 0.01, 4.0);
    Parameter pBeta ("Beta",  0.5, 0.01, 4.0);
    
    // NOW MAKE A FIT FUNCTION AND CONNECT IT TO ITS PARAMETERS:
    Variable X;
    Log log;
    Gamma gamma;
    Exp   exp;
    GENFUNCTION gammaDistribution=exp(-pAlpha*log(pBeta))/gamma(pAlpha)*exp(-X/pBeta+(pAlpha-1)*log(X));
    
    // MAKE A FUNCTIONAL, WHICH RETURNS THE LIKELIHOOD OF A DATA MODEL GIVEN A FIT FUNCTION
    HistLikelihoodFunctional objectiveFunction(&hist);
    GENFUNCTION f = hist.sum()*gammaDistribution;
    
    // NOW MAKE AN ENGINE FOR MINIMIZING THE LIKELIHOOD BY VARYING THE PARAMETERS.
    bool verbose=!silent;
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
    C(1,0)=minimizer.getError(&pBeta,&pAlpha);
    
    // STUFF THE DEVIATION INTO A VECTOR:
    Eigen::VectorXd V(2);
    V[0]=minimizer.getValue(&pAlpha)-alphaTrue;
    V[1]=minimizer.getValue(&pBeta) -betaTrue;
    
    // COMPUTE THE PROBILITY OF CHI^2
    double chiSquareAccuracy = (V.transpose()*C.inverse()*V)(0,0);
    double degreesOfFreedom=2;
    IncompleteGamma cumulativeChiSquareAccuracy=IncompleteGamma(IncompleteGamma::P);
    cumulativeChiSquareAccuracy.a().setValue(degreesOfFreedom/2.0); 
    double fitAccuracyProb=1-cumulativeChiSquareAccuracy(chiSquareAccuracy/2.0);
    

    // MONITOR FIT ACCURACY
    fitAccuracy.accumulate(fitAccuracyProb);
      

    // IF NOT IN SILENT MODE, LOOK AT THE RESULTS OF A SINGLE EXPERIMENT
    if (!silent)
    {

      // HISTOGRAM OF INPUT DATA:
      PlotHist1D pHist(hist);
      PlotHist1D::Properties prop;
      prop.brush.setStyle(Qt::SolidPattern);
      prop.pen.setWidth(1);
      prop.plotStyle=PlotHist1D::Properties::SYMBOLS;
      prop.symbolSize=6;
      pHist.setProperties(prop);

      // RESIZE THE PLOT VIEW
      {
	PRectF rect=pHist.rectHint();
	rect.setYmin(0.0);
	view1.setRect(rect);
      }
      
      // THE FITTING FUNCTION:
      PlotFunction1D pGamma(hist.sum()*gammaDistribution*hist.binWidth());
    
      // ADD HISTOGRAM AND FUNCTION TO THE PLOT
      view1.add(&pHist);
      view1.add(&pGamma);

      // ADD A LABEL KEY
      PlotKey key1(3,320);
      key1.setFont(font);
      key1.add(&pHist, "data");
      key1.add(&pGamma, "fit");
      view1.add(&key1);
      
      // EXPLICITLY SET THE PLOT VIEW LIMITS FOR PLOT 2
      PRectF rect;
      rect.setXmin(1.95);
      rect.setXmax(2.05);
      rect.setYmin(0.45);
      rect.setYmax(0.55);
      view2.setRect(rect);
      
      // SUMMARIZE THE FIT IN AN ERROR ELLIPSE:
      PlotErrorEllipse errorEllipse(minimizer.getValue(&pAlpha), 
				    minimizer.getValue(&pBeta),
				    minimizer.getError(&pAlpha,&pAlpha),
				    minimizer.getError(&pBeta, &pBeta),
				    minimizer.getError(&pAlpha,&pBeta),
				    PlotErrorEllipse::ONESIGMA);
      
      // PLOT ALSO THE TRUE VALUE OF ALPHA AND BETA
      PlotPoint trueValue(2.0, 0.5);
      {
	PlotPoint::Properties prop;
	prop.brush.setStyle(Qt::SolidPattern);
	trueValue.setProperties(prop);
      }
      
    
      // ADD THE FIT ACCURACY TO THE TITLE OF PLOT 2
      {
	PlotStream titleStream(view2.titleTextEdit());
	titleStream
	  << PlotStream::Clear() 
	  << PlotStream::Center() 
	  << PlotStream::Size(18)
	  << titleMap[&view2]
	  << fitAccuracyProb
	  << PlotStream::EndP();
      }

      // ADD ERROR ELLIPSE TO PLOT 2
      view2.add(&errorEllipse);
      view2.add(&trueValue);

      // ADD A LABEL KEY TO PLOT 2
      PlotKey key2(2.0,.53);
      key2.setFont(font);
      key2.add(&trueValue, "input value");
      key2.add(&errorEllipse, "measurement");
      view2.add(&key2);


      // PLOT/UPDATE THE FIT ACCURACY
      PlotHist1D pHistA(fitAccuracy);
      view3.add(&pHistA);
      view3.setRect(pHistA.rectHint());

      // INTERACT
      window.show();
      app.exec();

      // CLEAR PLOTS
      view1.clear();
      view2.clear();
      view3.clear();
    }
    
  }




  // SILENT OR NOT:  END WITH FIT ACCURACY PLOT
  PlotHist1D pHistA(fitAccuracy);
  view3.add(&pHistA);
  {
    PRectF rect=pHistA.rectHint();
    rect.setYmin(0.0);
    rect.setYmax(2.0*rect.ymax());
    view3.setRect(rect);
  }
  
  // Solid fill histograms
  PlotHist1D::Properties prop;
  prop.brush.setStyle(Qt::SolidPattern);
  prop.brush.setColor("lightGray");
  prop.pen.setWidth(1);
  pHistA.setProperties(prop);
  


  // ADD A LABEL KEY TO PLOT 3
  PlotKey key3(0.2, 1.5*pHistA.rectHint().bottom());
  key3.setFont(font);
  key3.add(&pHistA, "fit accuracy");
  view3.add(&key3);

  // DISABLE NEXT BUTTON
  nextAction->setEnabled(false);

  // INTERACT
  window.show();
  app.exec();

  
}
    


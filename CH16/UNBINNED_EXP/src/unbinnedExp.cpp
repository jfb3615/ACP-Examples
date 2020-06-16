#include "QatDataAnalysis/OptParse.h"
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
#include "QatGenericFunctions/Exp.h"
#include "QatGenericFunctions/NormalDistribution.h"
#include "QatGenericFunctions/Variable.h"
#include <QMainWindow>
#include <QApplication>
#include <QToolBar>
#include <QAction>
#include <QtGui>
#include <iostream>
#include <string>
#include <random>
using namespace Genfun;
int main(int argc, char ** argv ) {
  std::string usage = std::string(argv[0]) + " [NRUNS=val] [NPER=val]";
  NumericInput numeric;
  numeric.declare("NRUNS", "Number of runs", 10);
  numeric.declare("NPER",  "Number of events per run", 10000);
  numeric.optParse(argc, argv);
  
  if (argc!=1) {
    std::cout << usage << std::endl;
    exit(0);
  }
  
  unsigned int NRUNS = (unsigned int)(0.5+numeric.getByName("NRUNS"));
  unsigned int NPER  = (unsigned int)(0.5+numeric.getByName("NPER"));

  // SOME OVERHEAD TO GETTING THIS TO RUN IN A GUI:
  QApplication app(argc, argv);
  QMainWindow window;
  QToolBar *toolBar=window.addToolBar("Tools");
  QAction  *nextAction=toolBar->addAction("Next");
  nextAction->setShortcut(QKeySequence("n"));
  QObject::connect(nextAction, SIGNAL(triggered()), &app, SLOT(quit()));
  
  // INPUT VALUES TO ALPHA AND BETA
  double tau=3.7;
  
  // CONSTRUCT RANDOM NUMBER GENERATORS:
  unsigned int seed=999;
  std::mt19937 engine(seed);
  std::exponential_distribution<double> e(1.0/tau);

  // D0 SOME PSEUDOEXPERIMENTS:
  for (unsigned int i=0; i<NRUNS; i++) {

    // MAKE A TABLE AND COLLECT THE DATA:
    Table table("Data collection");
    for (unsigned int i=0;i<NPER;i++) {
      table.add("X",e(engine));
      table.capture();
    }
    // PREPARE THE FIT.  THIS IS A TWO-PARAMETER FIT.
    Parameter pTau("Tau", 0.5, 0.1, 10.0);
    Variable  X=table.symbol("X");
    std::cout << X.dimensionality() << std::endl;

    // NOW MAKE A FIT FUNCTION AND CONNECT IT TO ITS PARAMETERS:
    Exp exp;
    
    // MAKE A FUNCTIONAL, WHICH RETURNS THE LIKELIHOOD OF A DATA MODEL GIVEN A FIT FUNCTION
    TableLikelihoodFunctional objectiveFunction(table);
    GENFUNCTION f = exp(-X/pTau)/pTau;
    
    double y = f(3.2);
    std::cout << "Here is" << y  << std::endl;

    // NOW MAKE AN ENGINE FOR MINIMIZING THE LIKELIHOOD BY VARYING THE PARAMETERS.
    bool verbose=true;
    MinuitMinimizer minimizer(verbose);
    minimizer.addParameter(&pTau);
    minimizer.addStatistic(&objectiveFunction,&f);

    std::cout << "OF" << objectiveFunction (f) << std::endl;
    
    minimizer.minimize();
    
    // Fill a histogram:
    Hist1DMaker hMaker(X, 100, 0, 15);
    Hist1D hist = hMaker*table;

    // NOW LOOK AT THE RESULTS OF A SINGLE EXPERIMENT
    if (1)
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
      PlotFunction1D pTau(NPER*f*hist.binWidth());
    
      // MAKE A PLOT
      PlotView view(pHist.rectHint());
      window.setCentralWidget(&view);
      view.setBox(false);

      // ADD HISTOGRAM AND FUNCTO TO THE PLOT
      view.add(&pHist);
      view.add(&pTau);
      
      // CONSTRUCT THREE LABEL STREAMS..
      PlotStream titleStream(view.titleTextEdit()), xLabelStream(view.xLabelTextEdit()), yLabelStream(view.yLabelTextEdit());
      
      // LABEL THE TITLE:
      titleStream
	<< PlotStream::Clear() 
	<< PlotStream::Center() 
	<< "    Exp " 
	<< PlotStream::Family("Sans Serif")  
	<< PlotStream::Size(18)  
	<< "Distribution (Monte Carlo)" 
	<< PlotStream::EndP();
      
      // LABEL THE X-AXIS
      yLabelStream 
	<< PlotStream::Clear() 
	<< PlotStream::Family("Symbol")  
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
    if (1)
    {
      // MAKE A PLOT
      PRectF rect;
      rect.setXmin(2.5);
      rect.setXmax(4.5);
      rect.setYmin(-1);
      rect.setYmax(15);
      PlotView view(rect);
      window.setCentralWidget(&view);
      view.setBox(false);
      
      NormalDistribution G;
      G.mean().setValue(minimizer.getValue(&pTau));
      G.sigma().setValue(sqrt(minimizer.getError(&pTau,&pTau)));
      PlotFunction1D measuredValPlot(G);
      
      
      // PLOT ALSO THE TRUE VALUE OF ALPHA AND BETA:  SET SOME PROPERTIES,, TOO.
      PlotPoint trueValue(tau, 0.0);
      {
	PlotPoint::Properties prop;
	
	prop.brush.setColor(QColor("darkRed"));
	prop.brush.setStyle(Qt::SolidPattern);
	trueValue.setProperties(prop);
      }
      
    
      // CONSTRUCT THREE LABEL STREAMS..
      PlotStream  xLabelStream(view.xLabelTextEdit()), yLabelStream(view.yLabelTextEdit());
      
      xLabelStream 
	<< PlotStream::Clear() 
	<< PlotStream::Family("Symbol")  
	<< "\t\t\t\tt" 
	<< PlotStream::EndP();
      
      // LABEL THE Y-AXIS
      yLabelStream 
	<< PlotStream::Clear() 
	<< PlotStream::Family("Symbol")  
	<< "\t\t r" 
	<< PlotStream::EndP();
      
      // VIEW THE ERROR ELLIPSES
      view.add(&measuredValPlot);
      view.add(&trueValue);
      window.show();
      app.exec();
    }
  }

  return 0;

}

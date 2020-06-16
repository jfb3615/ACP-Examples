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
#include "QatPlotting/PlotKey.h"
#include "QatDataModeling/HistChi2Functional.h"
#include "QatDataModeling/HistLikelihoodFunctional.h"
#include "QatDataModeling/TableLikelihoodFunctional.h"
#include "QatDataModeling/MinuitMinimizer.h"
#include "QatGenericFunctions/Log.h"
#include "QatGenericFunctions/Exp.h"
#include "QatGenericFunctions/Gamma.h"
#include "QatGenericFunctions/NormalDistribution.h"
#include "QatGenericFunctions/Variable.h"
#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <QtGui>
#include <iostream>
#include <string>
#include <random>
using namespace Genfun;

class SignalCatcher: public QObject {
  Q_OBJECT
public slots:
  void quit() { exit (0);}
};

#include "unbinnedGauss.moc"

int main(int argc, char ** argv ) {

  // SOME OVERHEAD TO GETTING THIS TO RUN IN A GUI:
  QApplication app(argc, argv);
  QMainWindow window;
  QToolBar *toolBar=window.addToolBar("Tools");

  QAction  *nextAction=toolBar->addAction("Next");
  nextAction->setShortcut(QKeySequence("n"));
  QObject::connect(nextAction, SIGNAL(triggered()), &app, SLOT(quit()));

  SignalCatcher catcher;
  QAction  *quitAction=toolBar->addAction("Quit");
  quitAction->setShortcut(QKeySequence("q"));
  QObject::connect(quitAction, SIGNAL(triggered()), &catcher, SLOT(quit()));

  
  // INPUT VALUES TO ALPHA AND BETA
  double alphaTrue=3, betaTrue=1.25;
  
  // CONSTRUCT RANDOM NUMBER GENERATORS:
  unsigned int seed=999;
  std::mt19937 engine(seed);
  std::gamma_distribution<double> g(alphaTrue);

  // D0 SOME PSEUDOEXPERIMENTS:
  const unsigned int NPSEUDO=100;
  for (unsigned int i=0; i<NPSEUDO; i++) {

    // NUMBER OF EVENTS/EXPERIMENT
    const int NTOT=10000;
    
    // MAKE A TABLE AND COLLECT THE DATA:
    Table table("Data collection");
    for (int i=0;i<NTOT;i++) {
      table.add("X",g(engine)*betaTrue);
      table.capture();
    }
    
    // PREPARE THE FIT.  THIS IS A TWO-PARAMETER FIT.
    Parameter pM1 ("Mean1", 1.0, 0.1, 20.0);
    Parameter pM2 ("Mean2", 4.0, 0.1, 20.0);
    Parameter pM3 ("Mean3", 3.0, 0.1, 20.0);
    Parameter pS1 ("Sigm1", 1.0, 0.1, 10.0);
    Parameter pS2 ("Sigm2", 4.0, 0.1, 20.0);
    Parameter pS3 ("Sigm3", 3.0, 0.1, 20.0);
    Parameter PF1 ("Frac1", 0.5, 0.0, 1.0);
    Parameter PF2 ("Frac2", 0.5, 0.0, 1.0);
    Variable  X=table.symbol("X");

    // NOW MAKE A FIT FUNCTION AND CONNECT IT TO ITS PARAMETERS:
    NormalDistribution G1, G2, G3;
    G1.mean().connectFrom(&pM1);
    G2.mean().connectFrom(&pM2);
    G3.mean().connectFrom(&pM3);
    G1.sigma().connectFrom(&pS1);
    G2.sigma().connectFrom(&pS2);
    G3.sigma().connectFrom(&pS3);
    GENFUNCTION F=PF1*G1 + (1-PF1)*(PF2)*G2 + (1-PF1)*(1-PF2)*G3;
    
    // MAKE A FUNCTIONAL, WHICH RETURNS THE LIKELIHOOD OF A DATA MODEL GIVEN A FIT FUNCTION
    TableLikelihoodFunctional objectiveFunction(table);
    GENFUNCTION f = F(X);
    
    // NOW MAKE AN ENGINE FOR MINIMIZING THE LIKELIHOOD BY VARYING THE PARAMETERS.
    bool verbose=true;
    MinuitMinimizer minimizer(verbose);
    minimizer.addParameter(&pM1);
    minimizer.addParameter(&pM2);
    minimizer.addParameter(&pM3);
    minimizer.addParameter(&pS1);
    minimizer.addParameter(&pS2);
    minimizer.addParameter(&pS3);
    minimizer.addStatistic(&objectiveFunction,&f);
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
      prop.brush.setStyle(Qt::SolidPattern);
      prop.plotStyle=PlotHist1D::Properties::SYMBOLS;
      prop.symbolSize=6;
      pHist.setProperties(prop); 
    
      // MAKE A PLOT-ADAPTOR FOR THE FITTING FUNCTION:
      PlotFunction1D pGamma(NTOT*F*hist.binWidth());
      PlotFunction1D pGamm1(NTOT*PF1*G1*hist.binWidth());
      PlotFunction1D pGamm2(NTOT*(1-PF1)*PF2*G2*hist.binWidth());
      PlotFunction1D pGamm3(NTOT*(1-PF1)*(1-PF2)*G3*hist.binWidth());
      {
	PlotFunction1D::Properties prop;
	prop.pen.setWidth(3);
	prop.pen.setColor(QColor("darkGray"));
	pGamm1.setProperties(prop);
	pGamm2.setProperties(prop);
	pGamm3.setProperties(prop);
      }
      // Make the sum function a little darker:
      {
	PlotFunction1D::Properties prop;
	prop.pen.setWidth(3);
	pGamma.setProperties(prop);
      }
    
      // MAKE A PLOT
      PRectF rect;
      rect.setXmin(0.0);
      rect.setXmax(15.0);
      rect.setYmin(0.0);
      rect.setYmax(400.0);
      PlotView view(rect);
      window.setCentralWidget(&view);
      view.setBox(false);
      view.setXZero(false);
      view.setYZero(false);
      view.setGrid(false);

      // ADD HISTOGRAM AND FUNCTO TO THE PLOT
      view.add(&pGamma);
      view.add(&pGamm1);
      view.add(&pGamm2);
      view.add(&pGamm3);
      view.add(&pHist);

      
      PlotKey key(6,350);
      key.add(&pHist,"data");
      key.add(&pGamm1,"components 1,2,3");
      key.add(&pGamma,"sum");

      QFont font;
      font.setPointSize(16);
      key.setFont(font);
      view.add(&key);
      
      // CONSTRUCT THREE LABEL STREAMS..
      PlotStream xLabelStream(view.xLabelTextEdit()), yLabelStream(view.yLabelTextEdit());
      
      // LABEL THE X-AXIS
      yLabelStream 
	<< PlotStream::Clear() 
	<< PlotStream::Center() 
	<< PlotStream::Family("Sans Serif") 
	<< PlotStream::Size(16) 
	<< "ρ(x)" 
	<< PlotStream::EndP();
      
      // LABEL THE Y-AXIS
      xLabelStream 
	<< PlotStream::Clear() 
	<< PlotStream::Center() 
	<< PlotStream::Family("Sans Serif")  
	<< PlotStream::Size(16) 
	<< "x" 
	<< PlotStream::EndP();

      // SHOW THIS VIEW
      window.show();
      app.exec();
    
    }

  }

  return 0;

}

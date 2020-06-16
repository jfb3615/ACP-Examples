#include "QatPlotting/PlotStream.h"
#include "QatPlotWidgets/PlotView.h"
#include "QatPlotWidgets/MultipleViewWindow.h"
#include "QatDataAnalysis/Hist1D.h"
#include "QatDataAnalysis/Hist2D.h"
#include "QatDataAnalysis/Hist1DMaker.h"
#include "QatDataAnalysis/Hist2DMaker.h"
#include "QatDataAnalysis/Table.h"
#include "QatPlotting/RealArg.h"
#include "QatPlotting/PlotHist1D.h"
#include "QatPlotting/PlotHist2D.h"
#include "QatPlotting/PlotFunction1D.h"
#include "QatPlotting/PlotKey.h"
#include "QatDataModeling/TableLikelihoodFunctional.h"
#include "QatDataModeling/MinuitMinimizer.h"
#include "QatGenericFunctions/Variable.h"
#include "QatGenericFunctions/Exp.h"
#include "QatGenericFunctions/Log.h"
#include "QatGenericFunctions/Gamma.h"
#include "QatGenericFunctions/Parameter.h"
#include <QMainWindow>
#include <QToolBar>
#include <QApplication>
#include <QAction>
#include <QtGui>
#include <iostream>
#include <string>
#include <random>
#include <map>
using namespace Genfun;

class SignalCatcher:public QObject {
  Q_OBJECT
public slots:
  
  void quit() {exit(0);}
};
#include "multidim.moc"

int main(int argc, char ** argv ) {

  // SOME OVERHEAD TO GETTING THIS TO RUN IN A GUI:
  QApplication app(argc, argv);
  MultipleViewWindow window;
  QToolBar *toolBar=window.addToolBar("Tools");
  QAction  *nextAction=toolBar->addAction("Next");
  nextAction->setShortcut(QKeySequence("n"));
  QObject::connect(nextAction, SIGNAL(triggered()), &app, SLOT(quit()));

  SignalCatcher catcher;
  QAction  *quitAction=toolBar->addAction("Quit");
  quitAction->setShortcut(QKeySequence("q"));
  QObject::connect(quitAction, SIGNAL(triggered()), &catcher, SLOT(quit()));

  // INPUT VALUES TO ALPHA AND BETA
  double alpha0True=2.0, beta0True=1.0;
  double alpha1True=1.5, beta1True=1.8;
  
  // CONSTRUCT RANDOM NUMBER GENERATORS:
  unsigned int seed=999;
  std::mt19937 engine(seed);
  std::gamma_distribution<double> g0(alpha0True), g1(alpha1True);

  PlotView view1, view2, view3;

  std::map<PlotView *, std::string> xMap={{&view1, "x"}, {&view2,"x"}, {&view3, "y"}};
  std::map<PlotView *, std::string> yMap={{&view1, "y"}, {&view2,"ρ(x)"}, {&view3, "ρ(y)"}};
  for (PlotView *view : {&view1, &view2, &view3}) {
    view->setBox(false);
    view->setYZero(false);
    view->setXZero(false);
    view->setGrid(false);
    view->setFixedHeight(view->width());
    PlotStream xStream(view->xLabelTextEdit()), yStream(view->yLabelTextEdit());
    xStream << PlotStream::Clear()
	    << PlotStream::Center()
	    << PlotStream::Size(16)
	    << xMap[view]
	    << PlotStream::EndP();
    yStream << PlotStream::Clear()
	    << PlotStream::Center()
	    << PlotStream::Size(16)
	    << yMap[view]
	    << PlotStream::EndP();
    view->xAxisFont().setPointSize(16);
    view->yAxisFont().setPointSize(16);
  }
  window.add(&view1, "Joint pdf");
  window.add(&view2, "x- pdf");
  window.add(&view3, "y- pdf");

  // D0 SOME PSEUDOEXPERIMENTS:
  const unsigned int NPSEUDO=100;
  for (unsigned int i=0; i<NPSEUDO; i++) {

    // NUMBER OF EVENTS/EXPERIMENT
    const int NTOT=10000;
    
    // MAKE A TABLE AND COLLECT THE DATA:
    Table table("Data collection");
    for (int i=0;i<NTOT;i++) {
      table.add("X",g0(engine)*beta0True);
      table.add("Y",g1(engine)*beta1True);
      table.capture();
    }
    
    // PREPARE THE FIT.  THIS IS A TWO-PARAMETER FIT.
    Parameter pAlpha0("Alpha0", 0.5, 0.1, 3.5);
    Parameter pBeta0 ("Beta0",  0.5, 0.1, 3.5);
    Parameter pAlpha1("Alpha1", 0.5, 0.1, 3.5);
    Parameter pBeta1 ("Beta1",  0.5, 0.1, 3.5);
    Variable  X=table.symbol("X");
    Variable  Y=table.symbol("Y");
    
    Log log;
    Gamma gamma;
    Exp   exp;
    
    // Function of X
    Variable _x; // Dummy variable
    GENFUNCTION gammaDistribution0=
      exp(-pAlpha0*log(pBeta0))/gamma(pAlpha0)*exp(-_x/pBeta0+(pAlpha0-1)*log(_x));
    
    // Function of Y
    GENFUNCTION gammaDistribution1=
      exp(-pAlpha1*log(pBeta1))/gamma(pAlpha1)*exp(-_x/pBeta1+(pAlpha1-1)*log(_x));

    // Make a multivariate PDF
    Genfun::GENFUNCTION f = gammaDistribution0(X)*gammaDistribution1(Y); 

    // MAKE A FUNCTIONAL, WHICH RETURNS THE LIKELIHOOD OF A DATA MODEL GIVEN A FIT FUNCTION
    TableLikelihoodFunctional objectiveFunction(table);
    
    // NOW MAKE AN ENGINE FOR MINIMIZING THE LIKELIHOOD BY VARYING THE PARAMETERS.
    bool verbose=true;
    MinuitMinimizer minimizer(verbose);
    minimizer.addParameter(&pAlpha0);
    minimizer.addParameter(&pBeta0);
    minimizer.addParameter(&pAlpha1);
    minimizer.addParameter(&pBeta1);
    minimizer.addStatistic(&objectiveFunction,&f);
    minimizer.minimize();
    
    // Fill a histogram:
    Hist1DMaker hMakerX(X,100, 0, 15);
    Hist1D histX = hMakerX*table;
    

    Hist1DMaker hMakerY(Y, 100, 0, 15);
    Hist1D histY = hMakerY*table;


    Hist2DMaker hMakerXY(X, 25, 0, 5, Y, 25, 0, 5);
    Hist2D histXY=hMakerXY*table;

    // MAKE A PLOT ADAPTOR FOR THE HISTOGRAM AND SET UP SOME OF ITS PROPERTIES:
    PlotHist2D pHistXY(histXY);
    {
      PlotHist2D::Properties prop;
      prop.pen.setStyle(Qt::NoPen);
      prop.brush.setColor(QColor("darkGray"));
      prop.brush.setStyle(Qt::SolidPattern);
      pHistXY.setProperties(prop);
    }
    // MAKE A PLOT
    view1.setRect(pHistXY.rectHint());

    
    // ADD HISTOGRAM AND FUNCTO TO THE PLOT
    view1.add(&pHistXY);

    // MAKE A PLOT ADAPTOR FOR THE HISTOGRAM AND SET UP SOME OF ITS PROPERTIES:
    PlotHist1D pHistX(histX);
    {
      PlotHist1D::Properties prop;
      prop.pen.setWidth(1);
      prop.brush.setStyle(Qt::SolidPattern);
      prop.plotStyle=PlotHist1D::Properties::SYMBOLS;
      prop.symbolSize=6;
      pHistX.setProperties(prop);
    }
    // MAKE A PLOT-ADAPTOR FOR THE FITTING FUNCTION:
    PlotFunction1D pGamma0(NTOT*gammaDistribution0*histX.binWidth(),RealArg::Gt(0.0));
    
    // MAKE A PLOT
    view2.setRect(pHistX.rectHint());
    view2.setBox(false);
    
    // ADD HISTOGRAM AND FUNCTO TO THE PLOT
    view2.add(&pHistX);
    view2.add(&pGamma0);

    QFont f2; f2.setPointSize(16);
    PlotKey key2(8, 0.75*pHistX.rectHint().bottom());
    key2.setFont(f2);
    key2.add(&pHistX, "Data");
    key2.add(&pGamma0, "Fit projection");
    view2.add(&key2);
    
    // MAKE A PLOT ADAPTOR FOR THE HISTOGRAM AND SET UP SOME OF ITS PROPERTIES:
    PlotHist1D pHistY(histY);
    {
      PlotHist1D::Properties prop;
      prop.pen.setWidth(1);
      prop.brush.setStyle(Qt::SolidPattern);
      prop.plotStyle=PlotHist1D::Properties::SYMBOLS;
      prop.symbolSize=6;
      pHistY.setProperties(prop);
    }
    // MAKE A PLOT-ADAPTOR FOR THE FITTING FUNCTION:
    PlotFunction1D pGamma1(NTOT*gammaDistribution1*histY.binWidth(),RealArg::Gt(0.0));
    
    // MAKE A PLOT
    view3.setRect(pHistY.rectHint());
    view3.setBox(false);
    
    // ADD HISTOGRAM AND FUNCTO TO THE PLOT
    view3.add(&pHistY);
    view3.add(&pGamma1);

    QFont f3; f3.setPointSize(16);
    PlotKey key3(8, 0.75*pHistY.rectHint().bottom());
    key3.setFont(f3);
    key3.add(&pHistY, "Data");
    key3.add(&pGamma1, "Fit projection");
    view3.add(&key3);

    
    // SHOW THIS VIEW
    window.show();
    app.exec();
    view1.clear();
    view2.clear();
    view3.clear();
  }
  return 0;
}

#include "QatPlotWidgets/PlotView.h"
#include "QatPlotting/PlotStream.h"
#include "QatDataAnalysis/Hist1D.h"
#include "QatPlotting/PlotHist1D.h"
#include "QatPlotting/PlotFunction1D.h"
#include "QatGenericFunctions/Variable.h"
#include "QatGenericFunctions/Exp.h"
#include "QatGenericFunctions/Log.h"
#include "QatGenericFunctions/Sqrt.h"
#include "QatGenericFunctions/Tan.h"
#include "QatGenericFunctions/Abs.h"
#include "QatGenericFunctions/Theta.h"
#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <cstdlib>
#include <iostream>
#include <string>
#include <random>
int main (int argc, char * * argv) {
  //
  // Parse the example number from the command line (1,2, or 3):
  //
  unsigned int EX=0;
  std::string usage= std::string("usage: ") + argv[0] + " [-ex N (1,2 or 3)]"; 
  if (argc!=3) {
    std::cout << usage << std::endl;
    exit (0);
  }
  else if (argc==3) {
    if (std::string(argv[1])=="-ex") {
      std::istringstream stream(argv[2]);
      if (!(stream>>EX)) {
	std::cout << argv[0] << ": error parsing example number" << std::endl;
	exit(0);
      }
    }
    else {
      std::cout << usage << std::endl;
      exit (0);
    } 
  }

  if (EX<1 || EX>3) {
    std::cout << "Error, example number is 1 (exp), 2(cauchy), or 3 (triangular" << std::endl;
    exit (0);
  }
  
  std::random_device e;
  std::uniform_real_distribution<double> u(EX==3 ? -1.0:0.0,1.0);
  //
  // Plots & stuff:
  //

  QApplication     app(argc,argv);
  
  QMainWindow window;
  QToolBar *toolBar=window.addToolBar("Tools");
  QAction  *quitAction=toolBar->addAction("Quit");
  
  quitAction->setShortcut(QKeySequence("q"));
  
  QObject::connect(quitAction, SIGNAL(triggered()), &app, SLOT(quit()));
  
  // 
  // Construct a histogram for the data:
  //
  int minX=EX==1 ? 0:-10;
  Hist1D h=Hist1D("Rho(x)",      100, minX, 10);

  //
  // Some stuff we'll need:
  //
  double Tau=3.0;
  Genfun::Exp exp;
  Genfun::Log ln;
  Genfun::Tan tan;
  Genfun::Sqrt sqrt;
  Genfun::Abs abs;
  Genfun::Variable    X;

  // We also need a theta function
  Genfun::Theta theta;

  // 
  // PDF, Sampling Equation and title for the First example (exponential):
  // 

  Genfun::GENFUNCTION f1 =  (1/Tau)*exp(-X/Tau);   // Probability distribution
  Genfun::GENFUNCTION S1 =  -Tau*ln(1-X);          // Sampling equation:
  std::string title1 = "Exponential distribution";  // Title

  // 
  // PDF, Sampling Equation and title for the Second example (cauchy):
  // 

  Genfun::GENFUNCTION f2 =  (1/M_PI/(1+X*X));   // Probability distribution
  Genfun::GENFUNCTION S2 =  tan(M_PI*(X-0.5));  // Sampling equation:
  std::string title2 = "Cauchy/Breit-Wigner distribution";  // Title

  // 
  // PDF, Sampling Equation and title for the third example (triangular):
  // 

  
  Genfun::GENFUNCTION S30 =  sqrt(abs(4*X))-2;                             // Sampling equation:
  Genfun::GENFUNCTION f3  =  theta(2-abs(X)) * (
					       theta(-X)*0.25*(2+X) + theta(X)*0.25*(2-X)
					       );                          // Probability distribution
  Genfun::GENFUNCTION S3  =  theta(X)*S30(X) - theta(-X)*S30(-X);          // Sampling equation:
  std::string title3 = "Triangular distribution";  // Title

  Genfun::GENFUNCTION f = EX==1 ? f1:      EX==2 ? f2    : f3;  
  Genfun::GENFUNCTION S = EX==1 ? S1:      EX==2 ? S2    : S3;
  std::string     title = EX==1 ? title1:  EX==2 ? title2: title3;

  //
  // Fill the histogram:
  //
  const unsigned int N=100000;
  for (unsigned int i=0;i<N;i++) h.accumulate(S(u(e)));

  //
  // Plot PDF on top of the data. 
  //
  PlotHist1D pH=h;
  PlotFunction1D pF=(f)*N*h.binWidth();
  PlotFunction1D::Properties prop;
  prop.pen.setWidth(2);
  prop.pen.setColor("red");
  pF.setProperties(prop);
  


  PlotView view;
  window.setCentralWidget(&view);
  PlotStream titleStream(view.titleTextEdit());
  titleStream << PlotStream::Clear()
	      << PlotStream::Center() 
	      << PlotStream::Family("Sans Serif") 
	      << PlotStream::Size(16) 
	      << title
	      << PlotStream::EndP();
  
  
  PlotStream xLabelStream(view.xLabelTextEdit());
  xLabelStream << PlotStream::Clear()
	       << PlotStream::Center()
	       << PlotStream::Family("Sans Serif")
	       << PlotStream::Size(16)
	       << PlotStream::Italic()
	       << "x"
	       << PlotStream::EndP();
  
  PlotStream yLabelStream(view.yLabelTextEdit());
  yLabelStream << PlotStream::Clear()
	       << PlotStream::Center()
	       << PlotStream::Family("Symbol")
	       << PlotStream::Size(16)
	       << PlotStream::Italic()
	       << "r"
	       << PlotStream::Family("Sans serif")
	       << "(x)"
	       << PlotStream::EndP();
  
  
  view.add(&pH);
  view.add(&pF);
  view.setRect(pH.rectHint());
  window.show();
  view.show();
  app.exec();

  return 1;
}


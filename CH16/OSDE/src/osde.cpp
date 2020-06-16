#include "QatPlotWidgets/PlotView.h"
#include "QatPlotting/PlotStream.h"
#include "QatPlotting/PlotFunction1D.h"
#include "QatPlotting/PlotHist1D.h"
#include "QatDataAnalysis/Hist1D.h"
#include "QatDataAnalysis/OptParse.h"
#include "QatGenericFunctions/Sin.h"
#include "QatGenericFunctions/Sigma.h"
#include "QatGenericFunctions/Variable.h"
#include "QatGenericFunctions/FixedConstant.h"
#include "QatGenericFunctions/RombergIntegrator.h"
#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <cstdlib>
#include <iostream>
#include <string>
#include <random>
#include <stdexcept>
#include "Eigen/Dense"
int main (int argc, char * * argv) {

  using namespace std;
  using namespace Genfun;
  using namespace Eigen;
  //
  // Parse the input arguments:
  // N: number of events:
  // -h: show histogram
  // -t: show true distribution, according to which we generated
  // -r: show the distribution we have reconstructed
  //
  
  string usage=argv[0];
  usage += " [N=val/def=1000] [-r] [-t] [-h]  Val1 [Val 2] [Val 3] ...";
  
  if (argc==1) {
    cout << usage << endl;
    return 0;
  }

  
  NumericInput input; 
  unsigned int N=1000;
  input.declare("N", "Number of points to generate", N);
  try {
    input.optParse(argc, argv);
  }
  catch (const exception & e) {
    cerr << "Error parsing input" << endl;
    cerr << usage << endl;
    cerr << input.usage() << endl;
  }

  N= (unsigned int) (0.5+input.getByName("N"));
  
  bool truth=false, reco=false,histo=false;
  std::vector<double>  values;
  for (int argi=1;argi<argc;argi++) {
    if (argv[argi]==string("-r")) {
      reco=true;
      copy(argv+argi+1, argv+argc, argv+argi);
      argc -= 1;
      argi -= 1;
    }
    else if (argv[argi]==string("-t")) {
      truth=true;
      copy(argv+argi+1, argv+argc, argv+argi);
      argc -= 1;
      argi -= 1;
    }
    else if (argv[argi]==string("-h")) {
      histo=true;
      copy(argv+argi+1, argv+argc, argv+argi);
      argc -= 1;
      argi -= 1;
    }
    else {
      istringstream stream(argv[argi]);
      double val;
      if (!(stream >> val)) {
	cout << "Argument \"" << argv[argi] << "\" is not numeric. Try again." << endl;
	exit (0);
      }
      values.push_back(val);
    }
  }

  //
  // From the command line, construct a function with some Fourier components and
  // normalize it.  The function is called FNorm. 
  //
  Sigma sigma;
  Variable X;
  Sin      sin;
  for (unsigned int i=0;i<values.size();i++) {
    sigma.accumulate(values[i]*sqrt(2/M_PI)*sin((i+1)*X));
  }
  GENFUNCTION f=sigma*sigma;
  RombergIntegrator R(0,M_PI);
  GENFUNCTION FNorm=f/R(f);

  //
  // Generate Data, weight it by the function.
  // 
  typedef mt19937 EngineType;
  random_device rd;
  EngineType engine(rd());
  uniform_real_distribution<double> rand(0.0,M_PI);

  VectorXd vSum  =VectorXd::Zero(2*values.size());
  Hist1D h(100, 0, M_PI);
  double wSum=0.0;
  for (unsigned int i=0;i<N;i++) {
    double x = rand(engine);
    double y = FNorm(x);
    h.accumulate(x,y); // <== fill a weighted histogram.
    wSum+=y;
    for (unsigned int v=0;v<2*values.size();v++) {
      vSum[v] += y*sqrt(2/M_PI)*sin((v+1)*x);  // <==compute a weighed sum basis functions...
    }
  }
  for (unsigned int v=0;v<2*values.size();v++) {
    vSum[v]/=wSum; // <==divide out the sum of weight to obtain the average.
  }

  // Now you have the coefficients!

  //
  // From the coefficients, reconstitute a reconstructed density function:
  //
  Sigma sigmaReco;
  for (unsigned int v=0;v<2*values.size();v++) {
    sigmaReco.accumulate(vSum[v]*sqrt(2/M_PI)*sin((v+1)*X));
  }

  //
  // With the calculations done, we proceed to plot our results; histograms, true function
  // which was sampled, and function which was reconstructed, all in the same plotter, as
  // requested and steered by the command line options.
  //
  QApplication     app(argc,argv);
  
  QMainWindow window;
  QToolBar *toolBar=window.addToolBar("Tools");
  QAction  *nextAction=toolBar->addAction("Next");
  
  nextAction->setShortcut(QKeySequence("n"));
  
  QObject::connect(nextAction, SIGNAL(triggered()), &app, SLOT(quit()));
  

  
  PlotFunction1D pF=wSum*FNorm*h.binWidth();
  {
    PlotFunction1D::Properties prop;
    prop.pen.setColor("darkRed");
    prop.pen.setWidth(3);
    pF.setProperties(prop);
  }
  PlotHist1D p=h;
  PRectF rect=p.rectHint();
  rect.setYmin(-rect.ymax());

  PlotFunction1D pR=wSum*sigmaReco*h.binWidth();
  {
    PlotFunction1D::Properties prop;
    prop.pen.setColor("darkBlue");
    prop.pen.setWidth(3);
    pR.setProperties(prop);
  }
  
  PlotView view(rect);
  window.setCentralWidget(&view);
  
  if (histo) view.add(&p);
  if (truth) view.add(&pF);
  if (reco)  view.add(&pR);
  PlotStream titleStream(view.titleTextEdit());
  titleStream << PlotStream::Clear()
	      << PlotStream::Center() 
	      << PlotStream::Family("Sans Serif") 
	      << PlotStream::Size(16)
	      << "Orthogonal series density estimation"
	      << PlotStream::EndP();
  
  
  PlotStream xLabelStream(view.xLabelTextEdit());
  xLabelStream << PlotStream::Clear()
	       << PlotStream::Center()
	       << PlotStream::Family("Sans Serif")
	       << PlotStream::Size(16)
	       << "x"
	       << PlotStream::EndP();
  
  PlotStream yLabelStream(view.yLabelTextEdit());
  yLabelStream << PlotStream::Clear()
	       << PlotStream::Center()
	       << PlotStream::Family("Sans Serif")
	       << PlotStream::Size(16)
	       << "y"
	       << PlotStream::EndP();
  
  
  view.show();
  window.show();
  app.exec();
  return 1;
}


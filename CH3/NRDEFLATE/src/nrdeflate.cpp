#include "QatPlotWidgets/PlotView.h"
#include "QatPlotting/PlotStream.h"
#include "QatGenericFunctions/Variable.h"
#include "QatPlotting/PlotFunction1D.h"
#include "QatPlotting/PlotPoint.h"
#include "QatPlotting/PlotProfile.h"
#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <cstdlib>
#include <iostream>
#include <string>
#include <memory>

// The basic Newton-Raphson method.
double newtonRaphson(double x, Genfun::GENFUNCTION P) { 
  double x1=x;
  while (1) {
    double deltaX=-P(x)/P.prime()(x); 
    x+=deltaX;
    if (float(x1)==float(x)) break; 
    x1=x;
  }
  return x; 
}


//
// All of the work to deflate and plot is done in the signal catcher, which reacts to
// mouse clicks from the user.  
//
class SignalCatcher: public QObject{
  Q_OBJECT

public:

  SignalCatcher(PlotView *pV, Genfun::GENFUNCTION f);

private:
  
  PlotView *pV;
  Genfun::GENFUNCTION f;
  std::unique_ptr<Genfun::AbsFunction> deflatedF=std::unique_ptr<Genfun::AbsFunction> (f.clone());
  PlotFunction1D      P;           // Plot of: Original function
  PlotProfile         prf;         //          Set of zeros.
  std::unique_ptr<PlotFunction1D>     P1{nullptr}; //          Deflated function
  int                 iteration{0};
  void label();
	      
public slots:

  void deflate();
  
};

SignalCatcher::SignalCatcher(PlotView *pV, Genfun::GENFUNCTION f) :pV(pV),f(f),P(f){
  //
  // View properties(grid, axes visibility)
  pV->setXZero(false);
  pV->setGrid(false);
  //
  // Set styles for the curves...:
  {
    PlotFunction1D::Properties prop;
    prop.pen.setWidth(3);
    P.setProperties(prop);
  }
  // And for the intersection points:
  {
    PlotProfile::Properties prop;
    prop.brush.setStyle(Qt::SolidPattern);
    prop.brush.setColor("darkBlue");
    prop.symbolSize=10;
    prf.setProperties(prop);
  }
  // Add the original curve to the plot. 
  pV->add(&P);
  label();
}

// Set the x and y axes labels and the title:
void SignalCatcher::label() {
  // Plot labelling:
  const std::vector<std::string> title={"No roots found",
					"One root found",
					"Two roots found",
					"Three roots found",
					"Four roots found",
					"Five roots found"};
  
  PlotStream titleStream(pV->titleTextEdit());
  PlotStream xLabelStream(pV->xLabelTextEdit());
  PlotStream yLabelStream(pV->yLabelTextEdit());
  
  for (PlotStream * ps : {&titleStream, &xLabelStream,&yLabelStream }) {
    (*ps) <<
      PlotStream::Clear() <<
      PlotStream::Center()  <<
      PlotStream::Family("Sans Serif")  <<
      PlotStream::Size(16);
  }
  titleStream<< title[iteration] << PlotStream::EndP();
  xLabelStream << "x" << PlotStream::EndP();
  yLabelStream << "y=f(x)" << PlotStream::EndP();
  
}


void SignalCatcher::deflate() {

  // Clear and relabel the plot:
  if (iteration==5) exit(0);
  pV->clear();
  iteration++;
  label();

  //  find a root on the negative end of the range.
  double x = newtonRaphson(-1.0, *deflatedF);
  prf.addPoint(x,0);

  // Deflate
  Genfun::Variable X;
  Genfun::GENFUNCTION F1 = (*deflatedF)/(X-x);
  deflatedF=std::move(std::unique_ptr<Genfun::AbsFunction>(F1.clone()));

  // Create or refresh the plot of the updated deflated function:
  P1.reset(new PlotFunction1D(*deflatedF));
  {
    PlotFunction1D::Properties prop;
    prop.pen.setWidth(2);
    prop.pen.setStyle(Qt::DashLine);
    P1->setProperties(prop);
  }
  
  // Add this to the plot: 
  pV->add(&P);
  pV->add(P1.get());
  pV->add(&prf);
  
}





int main (int argc, char * * argv) {

  std::string usage= std::string("usage: ") + argv[0]; 
  if (argc!=1) {
    std::cout << usage << std::endl;
  }

  // Setup Qt:
  QApplication     app(argc,argv);

  // Window, toolbar and next action:
  QMainWindow window;
  QToolBar *toolBar=window.addToolBar("Tools");
  QAction  *nextAction=toolBar->addAction("Next");
  nextAction->setShortcut(QKeySequence("n"));

  // Set up a PlotView of specific dimensions:
  PlotView view({0.0,5.0, -2.0, 2.0});
  window.setCentralWidget(&view);

  // Choose an example function
  using namespace Genfun;
  Variable X;
  GENFUNCTION F=(X-1)*(X-2)*(X-3)*(X-M_PI)*(X-4);

  // All of the work is done in the signalCatcher, which responds to user mouse clicks:
  SignalCatcher signalCatcher(&view,F);
  QObject::connect(nextAction, &QAction::triggered, &signalCatcher, &SignalCatcher::deflate);

   
  // Show. Start the thread:
  view.show();
  window.show();
  app.exec();

  return 1;
}
#include "nrdeflate.moc"



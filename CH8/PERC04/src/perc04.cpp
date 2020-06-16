#include "PercolatorWidget.h"
#include "Percolator.h"
#include "QatPlotWidgets/PlotView.h"
#include "QatPlotting/PlotStream.h"
#include "QatPlotting/PlotProfile.h"
#include "QatPlotting/PlotHist1D.h"
#include "QatDataAnalysis/Hist1D.h"
#include "QatDataAnalysis/OptParse.h"
#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QTimer>
#include <QAction>
#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <unistd.h>
#include "Cluster.h"
//
// This class generates 2D configurations, efficiently clusters them and
// plots colorized clusters on a canvas.  This works well for lattices up
// to about 500 x 500.  
//

//
// The signal catcher resets the histogram when conditions change.
//
class SignalCatcher : public QObject {

  Q_OBJECT

public:
  
  SignalCatcher(Hist1D *h):h(h){}
  
public slots:


  void reset() {h->clear();}

private:

  Hist1D *h;
  
};

//
// The process has an evolution thread and a main thread which spies on
// the configuration generation and cluster reconstruction.  The threader
// is used in the evolution thread.  It agglomerates the objects needed to
// generate and cluster the configuration.
//
struct Threader {
  Percolator       *percolator;  // for Configuration generation and clustering
  PercolatorWidget *window;      // For Display
  Hist1D           *h;           // For cluster number plot
  Hist1D           *hSum;        // For cluster number plot.
  PlotHist1D       *pH;          // For disply of cluster number plot
};
bool finish=false;

// This runs in its own thread and evolves the system. 
void evolve(Threader *threader) {
	       
  unsigned long int nIter=0;
  while (!finish) {
    threader->percolator->next();
    threader->percolator->cluster();
    PercolatorWidget::mtx.lock();
    threader->window->updateImage();
    threader->window->setPercolates(threader->percolator->percolates());
    PercolatorWidget::mtx.unlock();
    for (unsigned int i=0;i<threader->percolator->getNumClusters();i++) {
      threader->h->accumulate(threader->percolator->getClusterSize(i)-0.5);
    }
    PercolatorWidget::mtx.lock();
    *threader->hSum = *threader->h;
    if (threader->hSum->sum()>1.0) {
      *threader->hSum *= 1/threader->hSum->sum();
    }
    PercolatorWidget::mtx.unlock();
    nIter++;
    threader->window->setIterations(nIter);
  }
}

int main (int argc, char * * argv) {

  
  // Parse the command line:

  std::string usage= std::string("usage: ") + argv[0] + " [N=val]"; 
  NumericInput input;
  input.declare ("N", "Lattice dimension", 400);
  try {
    input.optParse(argc, argv);
  }
  catch (std::exception & ) {
    std::cout << "usage" << std::endl;
    std::cout << input.usage() << std::endl;
    exit(0);
  }
  if (argc!=1) {
    std::cout << usage << std::endl;
    exit(0);
  }
  
  unsigned int N = (unsigned int) (0.5+input.getByName("N"));
  QApplication     app(argc,argv);
  
  // Most of the work is done in the percolator object:
  Percolator       percolator(N,N,0.5);
  // Display is handled in the percolator widget, a custom qt widget
  // that was created in part with QtDesigner:
  PercolatorWidget window(&percolator);

  // Histograms for cluster size distributions. 
  Hist1D h("Cluster size", 100, 0, 100);
  Hist1D hSum=h;
  PlotHist1D pH=hSum;
  
  // The "Threader" is loaded with pointer to the previously instantiated
  // objects and these are passed to the "evolve" routine which does the
  // actual work in its own thread.
  Threader threader;
  threader.window=&window;
  threader.percolator= &percolator;
  threader.h=&h;
  threader.hSum=&hSum;
  threader.pH=&pH;
  std::thread evolveThread (evolve,&threader); 

  // A timer is used to force update of the display.
  QTimer *timer=new QTimer;
  timer->setInterval(100); // milliseconds
    
  QObject::connect(timer, SIGNAL(timeout()), &window, SLOT(updatePixmap()));
  timer->start();

  // When the p-value changes, reset the histogram.
  SignalCatcher catcher(&h);
  QObject::connect(&window, SIGNAL(PChanged()), &catcher, SLOT(reset()));

  // Set the plot boundaries:
  PRectF rect;
  rect.setXmin(0.0);
  rect.setXmax(100.0);
  rect.setYmin(0.0);
  rect.setYmax(0.5);
  window.getClusterNumberView()->setRect(rect);
  window.getClusterNumberView()->add(&pH);
  window.show();
  app.exec();
  //
  // Done.  Stop the evolution thread. 
  //
  finish=true;
  evolveThread.join();
  return 1;
}

#include "perc04.moc"

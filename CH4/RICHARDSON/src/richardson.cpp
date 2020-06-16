#include "QatPlotWidgets/PlotView.h"
#include "QatPlotWidgets/MultipleViewWindow.h"
#include "QatPlotting/PlotKey.h"
#include "QatPlotting/PlotStream.h"
#include "QatPlotting/PlotProfile.h"
#include "QatPlotting/PlotFunction1D.h"
#include "QatGenericFunctions/FixedConstant.h"
#include <QApplication>
#include <QToolBar>
#include <QAction>
#include <cstdlib>
#include <iostream>
#include <string>
#include <cmath>
#include <queue>
#include <vector>
#include <memory>
using namespace std;

typedef std::unique_ptr<PlotProfile> PltPrfPtr;
#include "RichardsonTransformation.h"
  
  
int main (int argc, char * * argv) {
    
  
  // Automatically generated:-------------------------:
  
  string usage= string("usage: ") + argv[0]; 
  if (argc!=1) {
    cout << usage << endl;
  }
   
  QApplication     app(argc,argv);
  
  MultipleViewWindow window;
  QToolBar *toolBar=window.addToolBar("Tools");
  QAction  *quitAction=toolBar->addAction("Quit");
  
  quitAction->setShortcut(QKeySequence("q"));
  
  QObject::connect(quitAction, SIGNAL(triggered()), &app, SLOT(quit()));
  
  PRectF rect;

  rect.setXmin(0.0);
  rect.setXmax(20.0); 
  rect.setYmin(0.8);
  rect.setYmax(1.8);
  PlotView viewSeries(rect);
  window.add(&viewSeries, "Richardson Extrapolation");

  rect.setXmin(0.0);
  rect.setXmax(20.0); 
  rect.setYmin(1.0E-12);
  rect.setYmax(2.0);
  PlotView viewConvergence(rect);
  window.add(&viewConvergence, "Convergence Rate");

  const size_t SIZE=4;
  PltPrfPtr prof[SIZE], diff[SIZE];
  QColor   colors[SIZE]={"darkRed", "darkBlue", "darkGreen", "black"};
  PlotProfile::Properties prop;
  prop.brush.setStyle(Qt::SolidPattern);
  prop.symbolSize=8;
  
  for (size_t i=0;i<SIZE;i++) {
    prof[i]= PltPrfPtr(new PlotProfile);
    diff[i] = PltPrfPtr(new PlotProfile);
    prop.brush.setColor(colors[i%4]);
    prof[i]->setProperties(prop);
    diff[i]->setProperties(prop);
  }

  
  double sum1=0.0;
  Richardson richardson[SIZE]={Richardson(1), Richardson(2), Richardson(3), Richardson(4)} ;
  for (size_t i=0;i<20;i++) {
    sum1+=1.0/(i+1.0)/(i+1.0);;
    for (size_t s=0;s<SIZE;s++) {
      richardson[s].addPoint(sum1);
      Query<double> v=richardson[s].getRefinement();
      if (v.isValid() && isfinite((double) v)) {
	prof[s]->addPoint(i, richardson[s].getRefinement());
	diff[s]->addPoint(i, fabs(richardson[s].getRefinement()-M_PI*M_PI/6.0));
      }
    }
  }

  PlotView * view[]={&viewSeries, &viewConvergence};
  std::map<PlotView *, std::string > yLabelString={{&viewSeries,  "Sₙ"}, {&viewConvergence,  "Sₙ-S"}};


  PlotKey keyConverge(0.6, 1.0E-8);
  PlotKey keySeries(8, 1.2);
  viewSeries.add(&keySeries);
  viewConvergence.add(&keyConverge);
  viewSeries.yAxisFont().setPointSize(12);
  viewConvergence.yAxisFont().setPointSize(12);
  
  for (PlotView * v : view ) {
    PlotStream titleStream(v->titleTextEdit());
    titleStream << PlotStream::Clear()
		<< PlotStream::Center() 
		<< PlotStream::Family("Sans Serif") 
		<< PlotStream::Size(16)
		<< "Series approximation, li₂(1)"
		<< PlotStream::EndP();
    
    
    PlotStream xLabelStream(v->xLabelTextEdit());
    xLabelStream << PlotStream::Clear()
		 << PlotStream::Center()
		 << PlotStream::Family("Sans Serif")
		 << PlotStream::Size(16)
		 << "N" 
		 << PlotStream::EndP();
    
    PlotStream yLabelStream(v->yLabelTextEdit());
    yLabelStream << PlotStream::Clear()
		 << PlotStream::Center()
		 << PlotStream::Family("Sans Serif")
		 << PlotStream::Size(16)
		 << yLabelString[v]
		 << PlotStream::EndP();
  }
  PlotFunction1D target=Genfun::FixedConstant(M_PI*M_PI/6.0);


  viewSeries.add(&target);
  viewSeries.add(prof[0].get());
  viewSeries.add(prof[1].get());
  viewSeries.add(prof[2].get());
  //viewSeries.add(prof[3].get());
  keySeries.add(prof[0].get(), "Original Taylor series");
  keySeries.add(prof[1].get(), "Two point Richardson Extrapolation");
  keySeries.add(prof[2].get(), "Three point Richardson Extrapolation");
  //  keySeries.add(prof[3].get(), "Four Point Richardson Extrapolation");
  viewSeries.add(&keySeries);

  viewConvergence.add(diff[0].get());
  viewConvergence.add(diff[1].get());
  viewConvergence.add(diff[2].get());
  viewConvergence.add(diff[3].get());
  keyConverge.add(diff[0].get(), "Original Taylor series");
  keyConverge.add(diff[1].get(), "Two point Richardson Extrapolation");
  keyConverge.add(diff[2].get(), "Three point Richardson Extrapolation");
  keyConverge.add(diff[3].get(), "Four Point Richardson Extrapolation");

  viewConvergence.setLogY(true);
 
  window.show();
  app.exec();


  return 1;
}


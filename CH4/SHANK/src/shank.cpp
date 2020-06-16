#include "QatPlotWidgets/MultipleViewWindow.h"
#include "QatPlotWidgets/PlotView.h"
#include "QatPlotting/PlotStream.h"
#include "QatPlotting/PlotProfile.h"
#include "QatPlotting/PlotKey.h"
#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <cstdlib>
#include <iostream>
#include <string>
#include <cmath>
#include <queue>
#include <vector>
#include <map>
#include <memory>

using namespace std;

typedef std::unique_ptr<PlotProfile> PltPrfPtr;
#include "ShankTransformation.h"
  
  
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
  rect.setXmax(25.0);
  rect.setYmin(0.4);
  rect.setYmax(1.1);
  PlotView viewSeries(rect);
  rect.setXmin(0.0);
  rect.setXmax(25.0);
  rect.setYmin(1.0E-12);
  rect.setYmax(2.0);
  PlotView viewConvergence(rect);
  window.add(&viewSeries, "Taylor Series approx. to ln(2)");
  window.add(&viewConvergence, "Rate of Convergence");

  const size_t SIZE=4;
  PltPrfPtr prof[SIZE], diff[SIZE];
  QColor   colors[SIZE]={"darkRed", "darkBlue", "darkGreen", "black"};
  PlotProfile::Properties::SymbolStyle styles[]={PlotProfile::Properties::CIRCLE,
						 PlotProfile::Properties::TRIANGLE_U,
						 PlotProfile::Properties::TRIANGLE_L,
						 PlotProfile::Properties::SQUARE};
  Qt::BrushStyle bStyle[]={Qt::SolidPattern, Qt::SolidPattern, Qt::SolidPattern, Qt::SolidPattern};
  PlotProfile::Properties prop;
  prop.symbolSize=8;
  prop.pen.setWidth(2);
  
  for (size_t i=0;i<SIZE;i++) {
    prof[i]= PltPrfPtr(new PlotProfile);
    diff[i] = PltPrfPtr(new PlotProfile);
    prop.pen.setColor(colors[i%4]);
    prop.brush.setColor(colors[i%4]);
    prop.symbolStyle=styles[i%4];
    prop.brush.setStyle(bStyle[i%4]);
     prof[i]->setProperties(prop);
    diff[i]->setProperties(prop);
  }

  
  double log2=log(2.0);
  double sum1=0.0;
  Shank shankLog2(SIZE);
  double fact=-1.0;
  for (size_t i=0;i<25;i++) {
    fact*=(-1.0);
    sum1+=fact/(i+1);
    shankLog2.addPoint(sum1);
    for (size_t s=0;s<SIZE;s++) {
      if (i+1>2*s) prof[s]->addPoint(i, shankLog2.getRefinement(s));
      if (i+1>2*s) diff[s]->addPoint(i, fabs(shankLog2.getRefinement(s)-log2));
    }
  }
  PlotKey keyConverge(0.6, 1.0E-8);
  PlotKey keySeries(12, 1.0);
  viewSeries.add(&keySeries);
  viewConvergence.add(&keyConverge);
  viewSeries.yAxisFont().setPointSize(12);
  viewConvergence.yAxisFont().setPointSize(12);
  std::map<PlotView *, std::string > yLabelString={{&viewSeries,  "Sₙ"}, {&viewConvergence,  "Sₙ-S"}};

  for (PlotView * view : {&viewSeries, &viewConvergence} ) {
    PlotStream titleStream(view->titleTextEdit());
    titleStream << PlotStream::Clear()
		<< PlotStream::Center() 
		<< PlotStream::Family("Sans Serif") 
		<< PlotStream::Size(16)
		<< "Series approximation, log(2)"
		<< PlotStream::EndP();
    
    
    PlotStream xLabelStream(view->xLabelTextEdit());
    xLabelStream << PlotStream::Clear()
		 << PlotStream::Center()
		 << PlotStream::Family("Sans Serif")
		 << PlotStream::Size(16)
		 << "N" 
		 << PlotStream::EndP();
    
    PlotStream yLabelStream(view->yLabelTextEdit());
    yLabelStream << PlotStream::Clear()
		 << PlotStream::Center()
		 << PlotStream::Family("Sans Serif")
		 << PlotStream::Size(16)
		 << yLabelString[view]
		 << PlotStream::EndP();

  }
  window.show();

  viewSeries.add(prof[0].get());
  viewSeries.add(prof[1].get());
  //viewSeries.add(prof[2].get());
  //viewSeries.add(prof[3].get());
  keySeries.add(prof[0].get(), "Original Taylor series");
  keySeries.add(prof[1].get(), "One Shank Transformation");
  //  keySeries.add(prof[2].get(), "Two Shank Transformations");
  //  keySeries.add(prof[3].get(), "Three Shank Transformations");
  viewSeries.add(&keySeries);


  viewConvergence.add(diff[0].get());
  viewConvergence.add(diff[1].get());
  viewConvergence.add(diff[2].get());
  viewConvergence.add(diff[3].get());
  keyConverge.add(diff[0].get(), "Original Taylor series");
  keyConverge.add(diff[1].get(), "One Shank Transformation");
  keyConverge.add(diff[2].get(), "Two Shank Transformations");
  keyConverge.add(diff[3].get(), "Three Shank Transformations");
  viewConvergence.setLogY(true);
  app.exec();


  return 1;
}


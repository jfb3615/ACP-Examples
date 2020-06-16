#include "QatPlotWidgets/MultipleViewWindow.h"
#include "QatPlotWidgets/PlotView.h"
#include "QatPlotting/PlotStream.h"
#include "QatGenericFunctions/CubicSplinePolynomial.h"
#include "QatGenericFunctions/Log.h"
#include "QatGenericFunctions/Exp.h"
#include "QatGenericFunctions/FixedConstant.h"
#include "QatGenericFunctions/Sigma.h"
#include "QatPlotting/PlotFunction1D.h"
#include "QatPlotting/PlotProfile.h"
#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
int main (int argc, char * * argv) {

  // Automatically generated:-------------------------:

  std::string usage= std::string("usage: ") + argv[0] + " InputFile"; 
  if (argc!=2) {
    std::cout << usage << std::endl;
    return 1;
  }
  
  // Throw away the first three lines:
  const size_t SIZE=1024;
  char buff[SIZE];

  
  std::ifstream inputStream(argv[1]);
  inputStream.getline(buff,SIZE);
  inputStream.getline(buff,SIZE);
  inputStream.getline(buff,SIZE);

  std::vector<std::vector<double>> data;
  while ((inputStream.getline(buff, SIZE))) {
    std::string line=buff;
    std::istringstream lineStream(line);
    double datum;
    //
    // Allocate the arrays if they are not allocated:
    //
    if (data.empty()) {
      std::istringstream lineStream(line);  // shadowing...
      std::vector<double> tmp;
      while ((lineStream >> datum)) tmp.push_back(datum);
      data.resize(tmp.size());
    }
    size_t c(0);
    while ((lineStream >> datum)) {
      data[c++].push_back(datum);
    }
  }


  
  QApplication     app(argc,argv);
  
  MultipleViewWindow window;
  QToolBar *toolBar=window.addToolBar("Tools");
  QAction  *nextAction=toolBar->addAction("Next");
  
  nextAction->setShortcut(QKeySequence("n"));
  
  QObject::connect(nextAction, SIGNAL(triggered()), &app, SLOT(quit()));
  
  PRectF rect;
  rect.setXmin(0.001);
  rect.setXmax(100000);
  rect.setYmin(1E-6);
  rect.setYmax(40000.0);

  
  

  PlotView view(rect);
  view.setLogX(true);
  view.setLogY(true);
  view.setLabelXSizePercentage(75);

  PlotView verbView(rect);
  verbView.setLogX(true);
  verbView.setLogY(true);
  verbView.setLabelXSizePercentage(75);

  window.add(&view,      "Normal View");
  window.add(&verbView, "VerboseView");

  Genfun::Log log;
  Genfun::Exp exp;

  std::vector<QColor> colors = {"darkRed", "darkBlue", "black", "darkGreen", "magenta","olive"};
  
  Genfun::Sigma sigma;

  for (size_t p=4;p<7;p++) {
    Genfun::CubicSplinePolynomial cubicSpline;
    PlotProfile *prof=new PlotProfile;
    {
      PlotProfile::Properties prop;
      prop.symbolSize=10;
      prop.brush.setStyle(Qt::SolidPattern);
      prop.brush.setColor(colors[p-1]);
      prof->setProperties(prop);
    }
    for (size_t q=0;q<data[0].size();q++) {
      if (data[p][q]!=0.0) {
	cubicSpline.addPoint(log(data[0][q]), log(data[p][q]));
	prof->addPoint(data[0][q], data[p][q]);
      }
    }
    PlotFunction1D *pCubicSpline=NULL;
    if (p!=6) {
      pCubicSpline=new PlotFunction1D(exp(cubicSpline(log)));
      sigma.accumulate(exp(cubicSpline(log)));
    }
    else {
      pCubicSpline = new PlotFunction1D(sigma);
    }
    {
      PlotFunction1D::Properties prop;
      prop.pen.setWidth(3);
      prop.pen.setColor(colors[p-1]);
      pCubicSpline->setProperties(prop);
    }
    view.add(pCubicSpline);
    verbView.add(pCubicSpline);
    if (p<6) verbView.add(prof);
   
  }

  PlotFunction1D fAsymm=Genfun::FixedConstant(7.0/9.0/24.01);
  verbView.add(&fAsymm);
  view.add(&fAsymm);
  
  std::vector<PlotView *> viewSet={&view, &verbView};
  for (size_t v=0;v<viewSet.size();v++) {

    PlotStream statStream(viewSet[v]->statTextEdit());
    statStream << PlotStream::Clear()
	       << PlotStream::Left() 
	       << PlotStream::Family("Sans Serif") 
	       << PlotStream::Size(12)
	       << PlotStream::Color(colors[3])
	       << "― Pair production from nuclear field \n"
	       << PlotStream::Color(colors[4])
	       << "― Pair production from electron field \n"
	       << PlotStream::Color(colors[5])
	       << "― Total \n"
	       << PlotStream::EndP();


    PlotStream titleStream(viewSet[v]->titleTextEdit());
    titleStream << PlotStream::Clear()
		<< PlotStream::Center() 
		<< PlotStream::Family("Sans Serif") 
		<< PlotStream::Size(16)
		<< "Photons on Aluminium"
		<< PlotStream::EndP();
    
    
    PlotStream xLabelStream(viewSet[v]->xLabelTextEdit());
    xLabelStream << PlotStream::Clear()
		 << PlotStream::Center()
		 << PlotStream::Family("Sans Serif")
		 << PlotStream::Size(16)
		 << "E"
		 << PlotStream::Sub()
		 << "γ"
		 << PlotStream::Normal()
		 << " [MeV]"
		 << PlotStream::EndP();
    
    PlotStream yLabelStream(viewSet[v]->yLabelTextEdit());
    yLabelStream << PlotStream::Clear()
	       << PlotStream::Center()
		 << PlotStream::Family("Sans Serif")
		 << PlotStream::Size(16)
		 << "σ [cm²/g]"
		 << PlotStream::EndP();
  }
    
  window.show();
  app.exec();
  return 1;
}


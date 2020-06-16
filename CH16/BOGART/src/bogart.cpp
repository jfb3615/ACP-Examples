#include "QatPlotWidgets/PlotView.h"
#include "QatPlotting/PlotHist1D.h"
#include "QatPlotting/PlotFunction1D.h"
#include "QatDataAnalysis/Hist1D.h"
#include "QatDataAnalysis/OptParse.h"
#include "QatGenericFunctions/ArgumentList.h"

#include "F2DViewerWidget.h"
#include "FourierCoefficientSet.h"
#include "FourierExpansion.h"
#include "DualFourierCoefficientSet.h"
#include "DualFourierExpansion.h"
#include "OrthogonalSeriesWidget.h"

#include <Inventor/Qt/SoQt.h>
#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <QPixmap>
#include <QImage>
#include <QLabel>
#include <cstdlib>
#include <iostream>
#include <string>
#include <random>
#include <complex>

#include "ImageFunction.h"

// Seems proper to use one random engine and
// make it global.

std::random_device dev;
std::mt19937       engine(dev());

// This routine samples a distribution using the throwaway
// method. It throws on the intervals [0,xmax], [0, ymax], 
// and rejects on [0,zmax].  The number of points is determined
// by the size of the output vector argList, dimensioned before
// calling this routine. 
void sampleDistribution (Genfun::GENFUNCTION function, 
			   Genfun::ArgumentList & argList,
			   double xmax,
			   double ymax,
			   double zmax=1.0) {
  std::uniform_real_distribution<double> uX(0,xmax), uY(0,ymax), uZ(0,zmax);
  for (unsigned int i=0;i<argList.size();i++) {
    while (1) {
      Genfun::Argument arg(2); arg[0]=uX(engine); arg[1]=uY(engine);
      if  (uZ(engine) < function(arg)) {
	argList[i]=arg;
	break;
      }
    }
  }
}


int main (int argc, char * * argv) {

  // Automatically generated:-------------------------:

  std::string usage= std::string("usage: ") + argv[0] + " [NPOINTS=val] [NMAX=val] [-f filename]"; 

  unsigned int NPOINTS=10000;
  unsigned int NMAX=5;
  NumericInput input;
  input.declare("NPOINTS",  "Number of events",         NPOINTS);
  input.declare("NMAX",  "Number of Fourier components",   NMAX);
  input.optParse(argc, argv);
  NPOINTS = (unsigned int) (0.5 + input.getByName("NPOINTS"));
  NMAX = (unsigned int) (0.5 + input.getByName("NMAX"));
  
  std::string filename="bogart.jpg";
  for (int i=1; i<argc;i++) {
    if (std::string(argv[i])=="-f") {
      i++;
      filename=argv[i];
      std::copy (argv+i+1, argv+argc, argv+i-1);
      argc -=2;
      i    -=2;
    }
  }

  if (argc!=1) {
    std::cout << usage << std::endl;
  }



  QApplication     app(argc,argv);

  //
  // We read in the image that will define a function ("ImageFunction")
  //
  Genfun::ImageFunction iFunction(filename);
  int XMAX=iFunction.xmax();
  int YMAX=iFunction.ymax();  
  //
  // We sample this function 
  //
  Genfun::ArgumentList aList(NPOINTS);
  sampleDistribution (iFunction, aList, iFunction.xmax(), iFunction.ymax());
  std::cout << "Sampling complete" << std::endl;
  //
  // Make some histograms
  //
  Hist1D hx("X", XMAX, 0, XMAX);
  Hist1D hy("Y", YMAX, 0, YMAX);
  for (unsigned int i=0;i<aList.size();i++) {
    double x=aList[i][0],y=aList[i][1];
    hx.accumulate(x);
    hy.accumulate(y);
  }   
  //
  // Here we carry out the orthogonal series density estimation.  In this
  // demo it is done both in the joint PDF (x and y) and in both projections.
  // (Alternately one could analyze the joint pdf and then project).
  //
  static const std::complex<double> I(0,1);
  FourierCoefficientSet xCoefficients(NMAX),yCoefficients(NMAX);
  DualFourierCoefficientSet xyCoefficients(NMAX,NMAX);

  //
  // X and Y projection
  //
  for (unsigned int i=0;i<aList.size();i++) {
    double x=aList[i][0],y=aList[i][1];
    std::complex<double> eX0=exp(I*(M_PI*x/XMAX)),eX1=conj(eX0), eX =pow(eX0,NMAX);
    std::complex<double> eY0=exp(I*(M_PI*y/YMAX)),eY1=conj(eY0), eY =pow(eY0,NMAX);
    for (int l1=-int(NMAX);l1<=int(NMAX);l1++) {
      // Compute sums:
       xCoefficients(l1)         += sqrt(1.0/XMAX)/2.0 * eX; //exp(I*(-l1*M_PI*x/XMAX));
       yCoefficients(l1)         += sqrt(1.0/YMAX)/2.0  *eY; //exp(I*(-l1*M_PI*y/YMAX));
       eX *= eX1;
       eY *= eY1;
    }
  }
  //
  // XY Joint
  //
  for (unsigned int i=0;i<aList.size();i++) {
    double x=aList[i][0],y=aList[i][1];
    std::complex<double> eX0=exp(I*(M_PI*x/XMAX)),eX1=conj(eX0);
    std::complex<double> eY0=exp(I*(M_PI*y/YMAX)),eY1=conj(eY0);
    std::complex<double> eX =pow(eX0,NMAX);
    for (int l1=-int(NMAX);l1<=int(NMAX);l1++) {
      std::complex<double>  eY =pow(eY0,NMAX);
      for (int l2=-int(NMAX);l2<=int(NMAX);l2++) {
	xyCoefficients(l1,l2)         += 
	  1/(4.0*sqrt(XMAX)*sqrt(YMAX)) *
	  eX*eY;//exp(I*(-l1*M_PI*x/XMAX - l2*M_PI*y/YMAX));
	eY*=eY1;
      }
      eX*=eX1;
    }
  }

  //
  // Reduce our sums to averages:
  //   
  xCoefficients  *= (1.0/NPOINTS);
  yCoefficients  *= (1.0/NPOINTS);
  xyCoefficients *= (1.0/NPOINTS);
  //
  // Now we have coefficients of a double Fourier series and two simple
  // Fourier series.  Turn these into functions.  
  //
  Genfun::FourierExpansion xFunction(xCoefficients,XMAX);
  Genfun::FourierExpansion yFunction(yCoefficients,YMAX);
  Genfun::DualFourierExpansion xyFunction(xyCoefficients,XMAX,YMAX);
  Genfun::GENFUNCTION xyD=(XMAX*YMAX/2.0)*xyFunction;

  std::cout << "Computation complete" << std::endl;

  // 
  // Here we prepare to the graphics that we are going to plot:
  //  

  PlotHist1D pHX=hx;
  PlotHist1D pHY=hy;

  PlotFunction1D pSX=NPOINTS*xFunction,pSY=NPOINTS*yFunction;
  PlotFunction1D::Properties prop;
  prop.pen.setWidth(3);
  prop.pen.setColor("darkRed");
  pSX.setProperties(prop);
  pSY.setProperties(prop);

  QGraphicsPixmapItem *pixItem[2]={new QGraphicsPixmapItem, new QGraphicsPixmapItem};;
  for (int i=0;i<2;i++) {
    pixItem[i]->setPixmap(*iFunction.pixmap());
    pixItem[i]->setFlags(QGraphicsItem::ItemIsMovable);
    pixItem[i]->setScale(0.1);
    pixItem[i]->setPos(450,100);
  }

  //
  // Here we plot the graphics in a custom widget and interact
  //
  {
    SoQt::init("Orthogonal Series Density Estimation using Images");
    OrthogonalSeriesWidget window;

    window.getOriginal()->setSize(100, 0, XMAX, 100, 0, YMAX);
    window.getOriginal()->setFunction(&iFunction);

    window.getRealization()->setSize(100, 0, XMAX, 100, 0, YMAX);
    for (unsigned int i=0;i<aList.size();i++) window.getRealization()->addPoint(aList[i][0],aList[i][1], 1);

    window.getFit()->setSize(100, 0, XMAX, 100, 0, YMAX);
    window.getFit()->setFunction(&xyD);

    window.getXView()->setRect(pHX.rectHint());
    window.getXView()->add(&pHX);
    window.getXView()->add(&pSX);

    window.getYView()->setRect(pHY.rectHint());
    window.getYView()->add(&pHY);
    window.getYView()->add(&pSY);

    window.getXView()->scene()->addItem(pixItem[0]);
    window.getYView()->scene()->addItem(pixItem[1]);


    window.show();
    app.exec();
  }

  return 0;
}


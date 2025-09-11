#include "QatPlotWidgets/PlotView.h"
#include "QatPlotting/PlotStream.h"
#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <QLabel>
#include <QHBoxLayout> 
#include <QImage>
#include <cstdlib>
#include <iostream>
#include <string>
#include <Interpolator.h>

int main (int argc, char * * argv) {

  // Automatically generated:-------------------------:

  std::string usage= std::string("usage: ") + argv[0]; 
  if (argc!=1) {
    std::cout << usage << std::endl;
  }

  const int SIZE=420;
  
  QApplication     app(argc,argv);
  
  QMainWindow window;
  QToolBar *toolBar=window.addToolBar("Tools");
  QAction  *quitAction=toolBar->addAction("Quit");
  
  quitAction->setShortcut(QKeySequence("q"));
  
  QObject::connect(quitAction, SIGNAL(triggered()), &app, SLOT(quit()));
  
  
  QWidget *widget=new QWidget();
  widget->setFixedWidth(2*SIZE);
  widget->setFixedHeight(SIZE);
  window.setCentralWidget(widget);

  QHBoxLayout *layout=new QHBoxLayout;
  widget->setLayout(layout);
  QLabel *label1=new QLabel(widget);
  QLabel *label2=new QLabel(widget);
  label1->setFixedSize(SIZE,SIZE);
  layout->addWidget(label1);
  layout->addWidget(label2);
  label2->setFixedSize(SIZE,SIZE);
  
  QImage *image1=new QImage(SIZE,SIZE,QImage::Format_RGB32);
  QImage *image2=new QImage(SIZE,SIZE,QImage::Format_RGB32);



  
  Interpolator<GenInterpolator<double>> xyInterpolator;
  for (int i=0;i<SIZE;i+=SIZE/6) {
    double I=i-SIZE/2.0;
    double X=I/125.0;
    Interpolator<double>  yInterpolator;
    for (int j=0;j<SIZE;j+=SIZE/4) {
      double J=j-SIZE/2.0;
      double Y=J/125.0;
      double Z=0.5*std::hermite(5,X)*std::hermite(3,Y)+128;
      yInterpolator.addPoint(Y,Z);
    }
    xyInterpolator.addPoint(X,yInterpolator);
  }



  std::cout << "Interpolation done" << std::endl;
  for (int i=0;i<SIZE;i++) {
    for (int j=0;j<SIZE;j++) {
      double I=i-SIZE/2.0,J=j-SIZE/2.0;
      double X=I/125.0,Y=J/125.0;
      
      double Z=0.5*std::hermite(5,X)*std::hermite(3,Y)+128;
      {
	QRgb rgb=QColor(255,Z,255-Z,255).rgba();
	image1->setPixel(i,j,rgb);
      }
      {
	double W=xyInterpolator(X)(Y);
	QRgb rgb=QColor(255,W,255-W,255).rgba();
	image2->setPixel(i,j,rgb);
      }
    }
  }

  
  

  label1->setPixmap(QPixmap::fromImage(*image1));
  label2->setPixmap(QPixmap::fromImage(*image2));

  window.show();
  app.exec();
  return 1;
}


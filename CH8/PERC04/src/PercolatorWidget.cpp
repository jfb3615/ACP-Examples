#include "QatPlotting/PlotStream.h"
#include "PercolatorWidget.h"
#include "Percolator.h"
#include <QPixmap>
#include <QPalette>
#include <QImage>
#include <iostream>
class PercolatorWidget::Clockwork{
public:
  QImage     *image   =NULL;
  Percolator *percolator=NULL;
  unsigned int serialNo=0;
};
PercolatorWidget::PercolatorWidget(Percolator *percolator,QWidget *parent):
  QMainWindow(parent),c(new Clockwork())
{
  ui.setupUi(this);
  c->image =new QImage (percolator->NX(), percolator->NY(), QImage::Format_RGB32);

  ui.temperatureSpinBox->setValue(percolator->p());
  
  ui.actionQuit->setShortcut(QKeySequence("q"));
  ui.actionSaveImage->setShortcut(QKeySequence("s"));
  QObject::connect(ui.actionQuit, SIGNAL(triggered()), qApp, SLOT(quit()));
  QObject::connect(ui.temperatureSpinBox, SIGNAL(valueChanged(double)),
		   this, SLOT(setP(double)));
  QObject::connect(ui.actionSaveImage, SIGNAL(triggered()), this, SLOT(saveImage()));
  c->percolator=percolator;

  updateImage();
  
  ui.displayLabel->setPixmap(QPixmap::fromImage(*c->image));
  
  struct ViewStruct{
    PlotView *view;
    std::string title;
    std::string xLabel;
  };
  
  std::vector<ViewStruct> view={{ui.clusterNumberView,"Cluster Number 2D Lattice", "S"}};
				       
  for (unsigned int i=0;i<view.size();i++) {
    PlotStream titleStream(view[i].view->titleTextEdit());
    titleStream << PlotStream::Clear()
		<< PlotStream::Center() 
		<< PlotStream::Family("Sans Serif") 
		<< PlotStream::Size(16)
		<< view[i].title
		<< PlotStream::EndP();
    PlotStream xLabelStream(view[i].view->xLabelTextEdit());
    xLabelStream << PlotStream::Clear()
		<< PlotStream::Center() 
		<< PlotStream::Family("Sans Serif") 
		<< PlotStream::Size(16)
		<< view[i].xLabel
		<< PlotStream::EndP();
    PlotStream yLabelStream(view[i].view->yLabelTextEdit());
    yLabelStream << PlotStream::Clear()
		 << PlotStream::Center() 
		 << PlotStream::Family("Sans Serif") 
		 << PlotStream::Size(16)
		 << "N"
		 << PlotStream::Sub()
		 << "s"
		 << PlotStream::EndP();
  }

}
PercolatorWidget::~PercolatorWidget() {
  delete c;
}

PlotView        *PercolatorWidget::getClusterNumberView() {
  return ui.clusterNumberView;
}

void PercolatorWidget::updatePixmap() {

  mtx.lock();
  ui.displayLabel->setPixmap(QPixmap::fromImage(*c->image));
  ui.clusterNumberView->recreate();
  mtx.unlock();
}

void PercolatorWidget::updateImage () {
  for (unsigned int i=0;i<c->percolator->NX();i++) {
    for (unsigned int j=0;j<c->percolator->NY();j++) {
      bool on=c->percolator->isOccupied(i,j);
      unsigned int color=on ? c->percolator->getColor(c->percolator->getClusterId(i,j)):0;
      c->image->setPixel(i,j, color);
    }
  }
}

void PercolatorWidget::setP(double p) {
  c->percolator->p()=p;
  emit PChanged();
}


void PercolatorWidget::setIterations(int iter) {
  ui.iterationNumber->display(iter);
  ui.iterationNumber->update();
}

void PercolatorWidget::setPercolates(bool flag) {
  QPalette palette;
  palette.setColor(QPalette::Window, flag ? "green" : "red");
  palette.setColor(QPalette::Base, flag ? "green" : "red");
  ui.indicLabel->setPalette(palette);
  ui.indicLabel->setAutoFillBackground(true);
  ui.indicLabel->update();
}


void PercolatorWidget::saveImage() {
  std::ostringstream stream;
  stream << "perc_" << c->serialNo++ << ".png";
  c->image->save(stream.str().c_str());
}

std::mutex PercolatorWidget::mtx;

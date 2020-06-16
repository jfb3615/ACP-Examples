#include "QatPlotting/PlotStream.h"
#include "IsingModelWidget.h"
#include "IsingModel.h"
#include <QPixmap>
#include <QImage>
#include <iostream>
class IsingModelWidget::Clockwork{
public:
  QImage  *image   =NULL;
  IsingModel *model=NULL;
  bool       acquire=false;
};
IsingModelWidget::IsingModelWidget(IsingModel *model,QWidget *parent):
  QMainWindow(parent),c(new Clockwork())
{
  ui.setupUi(this);
  c->image =new QImage (model->NX(), model->NY(), QImage::Format_Mono);

  ui.temperatureSpinBox->setValue(model->tau());
  
  ui.actionQuit->setShortcut(QKeySequence("q"));
  QObject::connect(ui.actionQuit, SIGNAL(triggered()), qApp, SLOT(quit()));
  QObject::connect(ui.temperatureSpinBox, SIGNAL(valueChanged(double)),
		   this, SLOT(setTemperature(double)));
  QObject::connect(ui.acquireButton, SIGNAL(toggled(bool)), this, SLOT(setAcquire(bool)));
  c->model=model;

  for (unsigned int i=0;i<model->NX();i++) {
    for (unsigned int j=0;j<model->NY();j++) {
      bool on=model->isUp(i,j);
      c->image->setPixel(i,j,on);
    }
  }
  ui.displayLabel->setPixmap(QPixmap::fromImage(*c->image));

  struct ViewStruct{
    PlotView *view;
    std::string title;
    std::string xLabel;
  };
  std::vector<ViewStruct> view={
    {ui.uSeriesView,"Internal Energy", "iteration"},
    {ui.uVsTView, "Internal Energy vs Temperature", "T"},
    {ui.mSeriesView, "Magnetization","iteration"},
    {ui.mVsTView, "Magnetization vs. Temperature","T"}};
				       
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
		<< "fundamental units"
		<< PlotStream::EndP();
  }

}
IsingModelWidget::~IsingModelWidget() {
  delete c;
}

PlotView        *IsingModelWidget::getUSeriesView() {
  return ui.uSeriesView;
}

PlotView        *IsingModelWidget::getUVsTView(){
  return ui.uVsTView;
}

PlotView        *IsingModelWidget::getMSeriesView() {
  return ui.mSeriesView;
}

PlotView        *IsingModelWidget::getMVsTView(){
  return ui.mVsTView;
}


void IsingModelWidget::updatePixmap() {

  ui.displayLabel->setPixmap(QPixmap::fromImage(*c->image));

  ui.uSeriesView->recreate();
  ui.uVsTView->recreate();

  ui.mSeriesView->recreate();
  ui.mVsTView->recreate();
}

void IsingModelWidget::updateImage (unsigned int i, unsigned int j, bool on){
  c->image->setPixel(i,j,on);
}

void IsingModelWidget::setTemperature(double t) {
  c->model->tau()=t;
}


void IsingModelWidget::setIterations(int iter) {
  ui.iterationNumber->display(iter);
}

void IsingModelWidget::setAcquire(bool flag) {
  c->acquire=flag;
}

bool IsingModelWidget::getAcquire() const{
  return c->acquire;
}


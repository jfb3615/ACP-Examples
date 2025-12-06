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
};
IsingModelWidget::IsingModelWidget(IsingModel *model,QWidget *parent):
  QMainWindow(parent),c(new Clockwork())
{
  ui.setupUi(this);
  c->image =new QImage (model->NX(), model->NY(), QImage::Format_Mono);

  ui.temperatureSpinBox->setValue(model->tau());
  
  ui.actionQuit->setShortcut(QKeySequence("q"));
  QObject::connect(ui.actionQuit, &QAction::triggered, qApp, &QApplication::quit);
  QObject::connect(ui.temperatureSpinBox, &QDoubleSpinBox::valueChanged, this,  &IsingModelWidget::setTemperature);
  QObject::connect(ui.acquireButton, &QPushButton::toggled, this,  &IsingModelWidget::setAcquire);
  QObject::connect(ui.recordButton, &QPushButton::pressed, this, &IsingModelWidget::signalRecord);
  QObject::connect(ui.mkCorrButton, &QPushButton::pressed, this, &IsingModelWidget::signalMkCorr);
  
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
    std::string yLabel{"fundamental units"};
  };
  std::vector<ViewStruct> view={
    {ui.uSeriesView,"Internal Energy", "iteration"},
    {ui.uVsTView, "Internal Energy vs Temperature", "T"},
    {ui.mSeriesView, "Magnetization","iteration"},
    {ui.mVsTView, "Magnetization vs. Temperature","T"},
    {ui.uHistView, "Internal Energy", "U", "#states"},
    {ui.mHistView, "Magnetization", "M","#states"},
    {ui.correlationView, "Correlation Fcn", "r","#states"},
    {ui.cVsTView, "Heat Capacity", "C", "#states"},
    {ui.xVsTView, "Susceptibility", "χ","#states"}
  };
				       
  for (unsigned int i=0;i<view.size();i++) {
    PlotStream titleStream(view[i].view->titleTextEdit());
    titleStream << PlotStream::Clear()
		<< PlotStream::Center() 
		<< PlotStream::Family("Arial") 
		<< PlotStream::Size(16)
		<< view[i].title
		<< PlotStream::EndP();
    PlotStream xLabelStream(view[i].view->xLabelTextEdit());
    xLabelStream << PlotStream::Clear()
		<< PlotStream::Center() 
		<< PlotStream::Family("Arial") 
		<< PlotStream::Size(16)
		<< view[i].xLabel
		<< PlotStream::EndP();
    PlotStream yLabelStream(view[i].view->yLabelTextEdit());
    yLabelStream << PlotStream::Clear()
		<< PlotStream::Center() 
		<< PlotStream::Family("Arial") 
		<< PlotStream::Size(16)
		<< view[i].yLabel
		<< PlotStream::EndP();
  }
  ui.uSeriesView->setRect(PRectF{0.0,1000.0,-4.0,4.0});
  ui.uVsTView->setRect(PRectF{0.0,4.0,-4.0,4.0});
  ui.mSeriesView->setRect(PRectF{0.0,1000.0,-4.0,4.0});
  ui.mVsTView->setRect(PRectF{0.0,4.0,-4.0,4.0});
  ui.cVsTView->setRect(PRectF{0.0,4.0,-4,4});
  ui.xVsTView->setRect(PRectF{0.0,4.0,-4,4});

  ui.uHistView->setRect(PRectF{-4.0, 4.0, 0.0, 100.0});
  ui.mHistView->setRect(PRectF{-4.0,4.0, 0.0, 100.0});
  ui.correlationView->setRect(PRectF{0.0,200,-2.0, 2.0});

}
IsingModelWidget::~IsingModelWidget() {
  delete c;
}

PlotView        *IsingModelWidget::getUSeriesView() {
  return ui.uSeriesView;
}
PlotView        *IsingModelWidget::getUHistView() {
  return ui.uHistView;
}

PlotView        *IsingModelWidget::getUVsTView(){
  return ui.uVsTView;
}

PlotView        *IsingModelWidget::getMSeriesView() {
  return ui.mSeriesView;
}

PlotView        *IsingModelWidget::getMHistView() {
  return ui.mHistView;
}

PlotView        *IsingModelWidget::getCorrelationFcnView() {
  return ui.correlationView;
}

PlotView        *IsingModelWidget::getMVsTView(){
  return ui.mVsTView;
}

PlotView        *IsingModelWidget::getXVsTView(){
  return ui.xVsTView;
}

PlotView        *IsingModelWidget::getCVsTView(){
  return ui.cVsTView;
}


void IsingModelWidget::updatePixmap() {

  ui.displayLabel->setPixmap(QPixmap::fromImage(*c->image));

  ui.mSeriesView->recreate();
  ui.uSeriesView->recreate();
  ui.uHistView->recreate();
  ui.mHistView->recreate();
  ui.uVsTView->recreate();
  ui.cVsTView->recreate();
  ui.xVsTView->recreate();
  ui.xVsTView->recreate();
  ui.correlationView->recreate();
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

  if (flag) emit signalAcquire();
  else emit signalReset();
}


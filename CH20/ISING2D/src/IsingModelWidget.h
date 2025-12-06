#ifndef _ISINGMODELWIDGET_H_
#define _ISINGMODELWIDGET_H_

#include <QMainWindow>
#include "IsingModel.h"
#include "ui_IsingModelWidget.h"
class IsingModelWidget:public QMainWindow {

  Q_OBJECT

public:

  IsingModelWidget(IsingModel * model,QWidget *parent=NULL);
  ~IsingModelWidget();

  PlotView        *getUSeriesView();
  PlotView        *getUVsTView();
  PlotView        *getCVsTView();
  PlotView        *getXVsTView();
  
  PlotView        *getMSeriesView();
  PlotView        *getMVsTView();

  PlotView        *getUHistView();
  PlotView        *getMHistView();
  PlotView        *getCorrelationFcnView();
			     

public slots:

  void updatePixmap();
  void updateImage (unsigned int, unsigned int, bool);
  void setTemperature (double);
  void setIterations (int);

  void setAcquire(bool);
 
signals:

  void signalAcquire();
  void signalReset();
  void signalRecord();
  void signalMkCorr();
  
private:

  class Clockwork;
  Clockwork *c;

  Ui::IsingModelWidget ui;
  
  // Do not copy or assign:
  IsingModelWidget(const IsingModelWidget &)=delete;
  IsingModelWidget & operator = (const IsingModelWidget &)=delete;

};
#endif

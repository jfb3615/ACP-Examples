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

  PlotView        *getMSeriesView();
  PlotView        *getMVsTView();

  bool getAcquire() const;

public slots:

  void updatePixmap();
  void updateImage (unsigned int, unsigned int, bool);
  void setTemperature (double);
  void setIterations (int);

  void setAcquire(bool);


private:

  class Clockwork;
  Clockwork *c;

  Ui::IsingModelWidget ui;
  
  // Do not copy or assign:
  IsingModelWidget(const IsingModelWidget &)=delete;
  IsingModelWidget & operator = (const IsingModelWidget &)=delete;

};
#endif

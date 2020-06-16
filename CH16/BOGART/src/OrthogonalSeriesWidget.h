#ifndef _ORTHOGONALSERIESWIDGET_H_
#define _ORTHOGONALSERIESWIDGET_H_

#include <QMainWindow>
#include "ui_OrthogonalSeriesWidget.h"
class OrthogonalSeriesWidget:public QMainWindow {

  Q_OBJECT

public:

  OrthogonalSeriesWidget(QWidget *parent=NULL);
  ~OrthogonalSeriesWidget();

  F2DViewerWidget *getOriginal();
  F2DViewerWidget *getRealization();
  F2DViewerWidget *getFit();
  PlotView        *getXView();
  PlotView        *getYView();

public slots:


private:

  class Clockwork;
  Clockwork *c;

  Ui::OrthogonalSeriesWidget ui;
  
  // Do not copy or assign:
  OrthogonalSeriesWidget(const OrthogonalSeriesWidget &)=delete;
  OrthogonalSeriesWidget & operator = (const OrthogonalSeriesWidget &)=delete;

};
#endif

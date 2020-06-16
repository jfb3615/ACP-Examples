#ifndef _PERCOLATORWIDGET_H_
#define _PERCOLATORWIDGET_H_

#include <QMainWindow>
#include "Percolator.h"
#include "ui_PercolatorWidget.h"
#include <mutex>
class PercolatorWidget:public QMainWindow {

  Q_OBJECT

public:

  PercolatorWidget(Percolator * model,QWidget *parent=NULL);
  ~PercolatorWidget();

  PlotView        *getClusterNumberView();

  static  std::mutex   mtx;
  
public slots:

  void updatePixmap();
  void updateImage ();
  void setP (double);
  void setIterations (int);
  void setPercolates (bool);
  void saveImage();
  
 signals:
  void PChanged();
  
private:

  class Clockwork;
  Clockwork *c;

  Ui::PercolatorWidget ui;
  
  // Do not copy or assign:
  PercolatorWidget(const PercolatorWidget &)=delete;
  PercolatorWidget & operator = (const PercolatorWidget &)=delete;

};
#endif

#ifndef _POLYXTRAPOLATORWIDGET_H_
#define _POLYXTRAPOLATORWIDGET_H_
#include "ui_CubicSplineXtrapolatorWidget.h"
class CubicSplineXtrapolatorWidget:public QMainWindow {

  Q_OBJECT

public:

  CubicSplineXtrapolatorWidget(QWidget *parent=NULL, const std::string filename="");
  ~CubicSplineXtrapolatorWidget();

public slots:

  // Add a function:

  void quit();
  void add();
  void del();
  void functionSelect();
  void point(double x, double y);
  void refresh();

private:

  Ui::CubicSplineXtrapolatorWidget ui;

  class Clockwork;
  Clockwork *c;

  // Do not copy or assign:
  CubicSplineXtrapolatorWidget(const CubicSplineXtrapolatorWidget &);
  CubicSplineXtrapolatorWidget & operator = (const CubicSplineXtrapolatorWidget &);

};
#endif

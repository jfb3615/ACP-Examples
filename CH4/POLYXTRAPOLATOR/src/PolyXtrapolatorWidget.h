#ifndef _POLYXTRAPOLATORWIDGET_H_
#define _POLYXTRAPOLATORWIDGET_H_
#include "ui_PolyXtrapolatorWidget.h"
class PolyXtrapolatorWidget:public QMainWindow {

  Q_OBJECT

public:

  PolyXtrapolatorWidget(QWidget *parent=NULL, const std::string filename="");
  ~PolyXtrapolatorWidget();

public slots:

  // Add a function:

  void quit();
  void add();
  void del();
  void functionSelect();
  void point(double x, double y);
  void refresh();

private:

  Ui::PolyXtrapolatorWidget ui;

  class Clockwork;
  Clockwork *c;

  // Do not copy or assign:
  PolyXtrapolatorWidget(const PolyXtrapolatorWidget &);
  PolyXtrapolatorWidget & operator = (const PolyXtrapolatorWidget &);

};
#endif

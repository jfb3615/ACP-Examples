#ifndef _EULERFORM_H_
#define _EULERFORM_H_
#include "ui_eulerForm.h"
class QDial;
class QPushButton;
class EulerForm:public QWidget {

  Q_OBJECT

public:

  EulerForm(QWidget *parent=NULL);
  virtual ~EulerForm();
  
  QPushButton *crystalSphereButton() {return ui.crystalSphereButton;}
  QPushButton *realTimePushButton() {return ui.realTimePushButton;}
  
private:

  Ui::eulerForm ui;

  class Clockwork;
  Clockwork *c;

  // Do not copy or assign:
  EulerForm(const EulerForm &)=delete;
  EulerForm & operator = (const EulerForm &)=delete;

};

#endif

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
  
  QDial *phiDial() { return ui.phiDial;}
  QDial *thetaDial() { return ui.thetaDial;}
  QDial *psiDial() { return ui.psiDial;}
  QPushButton *resetButton() {return ui.resetButton;}
  
private:

  Ui::eulerForm ui;

  class Clockwork;
  Clockwork *c;

  // Do not copy or assign:
  EulerForm(const EulerForm &)=delete;
  EulerForm & operator = (const EulerForm &)=delete;

};

#endif

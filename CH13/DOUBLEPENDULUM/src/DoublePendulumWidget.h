// This class handles user interaction for the double pendulum
// widget which is a classroom demonstration.  The user interface
// was not written by hand but by QtDesigner...it's beyond the scope
// of the class; suffice it to say that it's the best way to build 
// a program with a significant user interface. 

#ifndef _DoublePendulumWidget_h_
#define _DoublePendulumWidget_h_

#include <QMainWindow>
#include "ui_DoublePendulumWidget.h"
class DoublePendulumWidget:public QMainWindow {

  Q_OBJECT
   
    public:

    //Constructor:
    DoublePendulumWidget (QWidget *parent=NULL);
    //Destructor 
    ~DoublePendulumWidget();

    public slots:

      void refresh();
      void quit();
      void changeParameter(double x);

    private:

      Ui::DoublePendulumWidget ui;

      class Clockwork;
      Clockwork *c;

      // Do not copy or assign:
      DoublePendulumWidget(const DoublePendulumWidget &);
      DoublePendulumWidget & operator = (const DoublePendulumWidget &);

};
#endif

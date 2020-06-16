#ifndef _SIGNAL_CATCHER_H
#define _SIGNAL_CATCHER_H
#include <QObject>
#include <iostream>
#include "QatGenericFunctions/Parameter.h"
 class SignalCatcher : public QObject
 {
     Q_OBJECT

 public:
   SignalCatcher():
   go(1),mu(NULL)
     {;}

   int go;
   int reset;

   Genfun::Parameter * mu;

 public slots:
   void toggleStopGo(){
     go = !go;
   }
   void restart() {
     reset=1;
   }
   void setMu(double x) {
     if (mu) mu->setValue(x);
     reset=1;
   }
 };
#endif

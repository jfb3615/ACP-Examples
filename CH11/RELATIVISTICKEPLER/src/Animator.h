#pragma once
#include <Inventor/sensors/SoTimerSensor.h>
#include "QatGenericFunctions/AbsFunction.h"
class SoSeparator;
class SoTranslation;
class SoPointSet;
class SoCoordinate3;

class Animator {
public:

  // Constructor
  Animator();

  // Destructor:
  ~Animator();

  // Adjust the point size in the trajectory trail;
  void setPointSize(unsigned int ps);

  // Adjust the time base.  Increase slows down animation
  void setTimeUnit(double tu);

  // Set the function (theta, and phi);
  void setXFunction(const Genfun::GENFUNCTION xFunction);
  void setYFunction(const Genfun::GENFUNCTION yFunction);
  void setZFunction(const Genfun::GENFUNCTION zFunction);

  // return the scene graph:
  SoSeparator *getSceneGraph() const;

  // Called back. First argument is the This pointer.
  static void timeSensorCallback(void * data , SoSensor * );
  
private:

  int step{0};                  // ticks off time
  int pS  {3};                  // Point size;
  double timeUnit  {1.0};       // Time unit, increase to slow down:
  SoTimerSensor timeSensor;     // The time sensor
  SoSeparator   *root{nullptr}; // Scene graph

  // Keep these in scope since they are updated:
  SoTranslation             *trans{nullptr};
  SoPointSet                *pointSet{nullptr};
  SoCoordinate3             *coordinates{nullptr};
  const Genfun::AbsFunction *x{nullptr};
  const Genfun::AbsFunction *y{nullptr};
  const Genfun::AbsFunction *z{nullptr};

};

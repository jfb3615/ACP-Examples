#ifndef _DOG_H_
#define _DOG_H_
#include "ZOO/Mammal.h"
class Dog: public Mammal {

 public:

  Dog(const std::string & name);
  virtual void play();

  virtual void eat();
  
  virtual void haveTypicalDay();

};
#endif

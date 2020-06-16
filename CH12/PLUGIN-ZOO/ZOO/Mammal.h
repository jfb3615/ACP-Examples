#ifndef _Mammal_H_
#define _Mammal_H_
#include "ZOO/Animal.h"
class Mammal: public Animal {

 public:

  Mammal(const std::string & name);
  virtual void lactate();


  
};
#endif

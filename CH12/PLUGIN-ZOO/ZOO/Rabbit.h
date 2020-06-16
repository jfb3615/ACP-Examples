#ifndef _RABBIT_H_
#define _RABBIT_H_
#include "ZOO/Mammal.h"
class Rabbit: public Mammal {

 public:

  Rabbit(const std::string & name);
  void procreate();

};
#endif

#ifndef _FISH_H_
#define _FISH_H_
#include "ZOO/Animal.h"

class Fish: public Animal {

 public:

  Fish(const std::string & name);
  void breatheThroughGills();

};
#endif

#ifndef _WHALE_H_
#define _WHALE_H_
#include "ZOO/Mammal.h"
class Whale: public Mammal {

 public:

  Whale(const std::string & name);
  virtual void spout();

  virtual void eat();
  
  virtual void haveTypicalDay();

};
#endif

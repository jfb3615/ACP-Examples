#ifndef _ANIMAL_H_
#define _ANIMAL_H_
#include <iostream>
#include <string>
class Animal {

 public:
// constructor:

  Animal (const std::string & name);
  
  // get the name:
  const std::string  & getName() const;
  
  // daily activities:
  virtual void wakeUp();
  virtual void eat();
  virtual void sleep();
  virtual void haveTypicalDay();
  
private:
	std::string  _name;
};
#endif

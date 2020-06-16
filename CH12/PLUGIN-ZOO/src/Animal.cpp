#include "ZOO/Animal.h"

Animal::Animal (const std::string & name):_name(name){
}

// get the name:
const std::string  & Animal::getName() const {
  return _name;
}

// daily activities:
void Animal::wakeUp() {
  std::cout << "Animal " << _name << " wakingUp" << std::endl;
}

void Animal::eat() {
  std::cout << "Animal " << _name << " eating" << std::endl;
}

void Animal::sleep() {
  std::cout << "Animal " << _name << " sleeping" << std::endl;
}
void Animal::haveTypicalDay() {
  eat();
  sleep();
}


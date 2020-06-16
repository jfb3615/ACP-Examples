#include "ZOO/Dog.h"

Dog::Dog(const std::string & name):Mammal(name) {
}
void Dog::play() {
  std::cout << "Dog " << getName() << " playing" << std::endl;
}


void Dog::eat() {
  std::cout << "Dog " << getName() << " eating" << std::endl;
}



void Dog::haveTypicalDay() {
  wakeUp();
  eat();
  lactate();
  play();
  sleep();
}


extern "C" Dog *makeDog() {
  return new Dog("Gus");
}

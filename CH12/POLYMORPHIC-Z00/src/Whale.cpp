#include "ZOO/Whale.h"

Whale::Whale(const std::string & name):Mammal(name) {
}

void Whale::eat() {
  std::cout << "Whale " << getName() << " eating KRILL" << std::endl;
}


void Whale::spout() {
  std::cout << "Whale " << getName() << " spouting" << std::endl;
}



void Whale::haveTypicalDay() {
  wakeUp();
  eat();
  spout();
  sleep();
  spout();
  sleep();
  spout();
  sleep();
  lactate();
  sleep();
}

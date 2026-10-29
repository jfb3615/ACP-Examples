#include "ZOO/Rabbit.h"

Rabbit::Rabbit(const std::string & name):Mammal(name) {
}
void Rabbit::hopAbout() {
  std::cout << "Rabbit " << getName() << " hopping about" << std::endl;
}

extern "C" Rabbit *makeRabbit() {
  return new Rabbit("Bugs");
}

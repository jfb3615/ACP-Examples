#include "ZOO/Rabbit.h"

Rabbit::Rabbit(const std::string & name):Mammal(name) {
}
void Rabbit::procreate() {
  std::cout << "Rabbit " << getName() << " procreating" << std::endl;
}

extern "C" Rabbit *makeRabbit() {
  return new Rabbit("Bugs");
}

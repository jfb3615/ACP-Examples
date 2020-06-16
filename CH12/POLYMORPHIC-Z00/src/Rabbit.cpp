#include "ZOO/Rabbit.h"

Rabbit::Rabbit(const std::string & name):Mammal(name) {
}
void Rabbit::procreate() {
  std::cout << "Rabbit " << getName() << " procreating" << std::endl;
}

void Rabbit::eat() {
  std::cout << "Rabbit " << getName() << " eating CARROTS" << std::endl;
}

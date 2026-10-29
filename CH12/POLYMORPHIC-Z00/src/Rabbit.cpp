#include "ZOO/Rabbit.h"

Rabbit::Rabbit(const std::string & name):Mammal(name) {
}
void Rabbit::hopAbout() {
  std::cout << "Rabbit " << getName() << " hopping about" << std::endl;
}

void Rabbit::eat() {
  std::cout << "Rabbit " << getName() << " eating CARROTS" << std::endl;
}

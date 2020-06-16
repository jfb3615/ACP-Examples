#include "ZOO/Mammal.h"

Mammal::Mammal(const std::string & name):Animal(name) {
}
void Mammal::lactate() {
  std::cout << "Mammal " << getName() << " lactating" << std::endl;
}

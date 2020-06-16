#include "ZOO/Fish.h"

Fish::Fish(const std::string & name):Animal(name) {
}
void Fish::breatheThroughGills() {
  std::cout << "Fish " << getName() << " breathing through gills" << std::endl;
}

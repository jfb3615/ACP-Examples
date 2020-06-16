#include "ZOO/Fish.h"
#include <libgen.h>

Fish::Fish(const std::string & name):Animal(name) {
}
void Fish::breatheThroughGills() {
  std::cout << "Fish " << getName() << " breathing through gills" << std::endl;
}



extern "C" Fish *makeFish() {
  return new Fish("Robert");
}

#include "Vect3D.h"
#include <iostream>
int main() {
  
  Vect3D a(1,-4,3),b(2,7,-8),c=a+b;

  std::cout << a << "+" << std::endl << b << "=" << std::endl << c << std::endl;
  
  return 0;
}

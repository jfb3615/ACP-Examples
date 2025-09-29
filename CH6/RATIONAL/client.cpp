#include "Rational.h"
#include <iostream>

void print(Rational r) {
  std::cout         << 
    "Numerator :"   << 
    r.num()         << 
    " denominator " << 
    r.den()         << 
    " value "       << 
    r.value()       << std::endl;
}

int main() {
  Rational r=3.14159;
  Rational a(1,2);
  Rational b(1,6);
  Rational z;
  print(z);
  print (r);
  print(a);
  print(b);
  return 1;
}

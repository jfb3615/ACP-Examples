//
// Program to add two vectors.
//
#include <iostream>
void vectorSum(double a0, double a1, double a2,
	       double b0, double b1, double b2,
	       double & c0, double & c1, double & c2) {
  c0=a0+b0;
  c1=a1+b1;
  c2=a2+b2;
}

int main() {
  double a0=1,a1=-4,a2=3;
  double b0=2,b1=7,b2=-8;
  double c0,c1,c2;
  
  vectorSum(a0,a1,a2,
	    b0,b1,b2,
	    c0,c1,c2);
  
  std::cout << a0 << " " << a1 << " " << a2 << "+" << std::endl;
  std::cout << b0 << " " << b1 << " " << b2 << "=" << std::endl;
  std::cout << c0 << " " << c1 << " " << c2 << std::endl;
  return 0;
}

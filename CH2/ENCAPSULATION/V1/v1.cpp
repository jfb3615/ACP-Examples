//
// Program to add two vectors.
//
#include <iostream>
struct Vect3D{
  double _x;
  double _y;
  double _z;
};

std::ostream & operator << (std::ostream & o, const Vect3D & v) {
  return o << v._x << " " << v._y << " " << v._z ;
}

Vect3D vectorSum(const Vect3D & a,
	       const Vect3D & b) {
  Vect3D c;
  c._x=a._x + b._x;
  c._y=a._y + b._y;
  c._z=a._z + b._z;
  return c;
}

int main() {

  Vect3D a,b;
  a._x=1,a._y=-4,a._z=3;
  b._x=2,b._y=7,b._z=-8;
  
  Vect3D c=vectorSum(a,b);
  
  std::cout << a << "+" << std::endl << b << "=" << std::endl << c << std::endl;

  return 0;
}

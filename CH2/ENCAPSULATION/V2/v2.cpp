//
// Program to add two vectors.
//
#include <iostream>
struct Vect3D{
  double _x;
  double _y;
  double _z;

  // Constructor
  Vect3D(double x, double y, double z):_x(x),_y(y),_z(z) {}

  // Constructor
  Vect3D():_x(0),_y(0),_z(0) {}
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

  Vect3D a(1,-4,3),b(2,7,-8), c=vectorSum(a,b);
  
  std::cout << a << "+" << std::endl << b << "=" << std::endl << c << std::endl;
  
  return 0;
}

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

  // Vector addition:
  Vect3D operator+(const Vect3D & right) const;
  
};

std::ostream & operator << (std::ostream & o, const Vect3D & v) {
  return o << v._x << " " << v._y << " " << v._z ;
}

Vect3D Vect3D::operator + (const Vect3D & right) const {
  Vect3D c;
  c._x=_x + right._x;
  c._y=_y + right._y;
  c._z=_z + right._z;
  return c;
}

int main() {

  Vect3D a(1,-4,3),b(2,7,-8);
  Vect3D c=a+b+a+b+b;

  std::cout << a << "+" << std::endl << b << "=" << std::endl << c << std::endl;
  
  return 0;
}

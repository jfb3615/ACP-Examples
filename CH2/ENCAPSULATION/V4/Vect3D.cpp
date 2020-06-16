#include "Vect3D.h"
#include <iostream>
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


#ifndef _VECT3D_H_
#define _VECT3D_H_
//
// Program to add two vectors.
//
#include <iostream>
class Vect3D{

public:
  
  // Constructor
  Vect3D(double x, double y, double z):_x(x),_y(y),_z(z) {}

  // Constructor
  Vect3D():_x(0),_y(0),_z(0) {}

  // Vector addition:
  Vect3D operator+(const Vect3D & right) const;
  

private:
  
  double _x;
  double _y;
  double _z;

  friend std::ostream & operator << (std::ostream & o, const Vect3D & v);
};

std::ostream & operator << (std::ostream & o, const Vect3D & v) ;

#endif

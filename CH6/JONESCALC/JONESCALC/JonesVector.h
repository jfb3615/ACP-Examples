#ifndef _JONESVECTOR_H_
#define _JONESVECTOR_H_
#include <complex>
#include <iostream>
typedef std::complex<double> Complex;
class JonesVector {

 public:

  enum Type {Horizontal,Vertical,Diagonal,Antidiagonal,Right,Left};

  // Construct a zero vector:
  inline JonesVector();

  // Construct a type from a predefined type
  inline JonesVector(Type type);

  // Construct on two complex numbers:
  inline JonesVector (const Complex &x0, const Complex & x1);

  // Compute the magnitude square (=intensity) :
  inline double magsq() const;

  // Access to individual elements:
  inline Complex & operator () (unsigned int i);
  inline const Complex & operator () (unsigned int i) const;

 private:

  Complex x0,x1;

};

inline std::ostream & operator << (std::ostream & o, const JonesVector & v);
inline JonesVector operator* (const JonesVector & v,        const Complex &c);
inline JonesVector operator* (const Complex & c,       const JonesVector & v);
#include "JONESCALC/JonesVector.icc"
#endif

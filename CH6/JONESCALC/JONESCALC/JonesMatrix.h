#ifndef _JONESMATRIX_H_
#define _JONESMATRIX_H_
#include <complex>
typedef std::complex<double> Complex;
class JonesMatrix {

 public:

  enum Type { Horizontal,Vertical,Diagonal,
	      Antidiagonal,Right,Left,
	      Identity, FastHorizontal, FastVertical};

  // Construct the identity matrix:
  inline JonesMatrix();

  // Construct a type from a predefined type
  inline JonesMatrix(Type type);

  // Construct on two complex numbers:
  inline JonesMatrix (const Complex &a00, const Complex & a01,
		      const Complex &a10, const Complex & a11);
  
  // Access to individual elements:
  inline Complex & operator       () (unsigned int i, unsigned int j);
  inline const Complex & operator () (unsigned int i, unsigned int j) const;

 private:

  Complex a00,a01,a10,a11;

};

inline std::ostream & operator << (std::ostream & o, const JonesMatrix & v);
inline JonesMatrix operator* (const JonesMatrix & m1, const JonesMatrix & m2);
inline JonesVector operator* (const JonesMatrix & m, const JonesVector & v);
#include "JONESCALC/JonesMatrix.icc"
#endif

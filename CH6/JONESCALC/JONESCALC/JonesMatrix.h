#ifndef _JONESMATRIX_H_
#define _JONESMATRIX_H_
#include <complex>
using Complex=std::complex<double>;
class JonesMatrix {

 public:

  enum Type { Horizontal,Vertical,Diagonal,
	      Antidiagonal,Right,Left,
	      Identity, FastHorizontal, FastVertical};

  // Construct the identity matrix:
  inline JonesMatrix()=default;

  // Construct a type from a predefined type
  inline JonesMatrix(Type type);

  // Construct on two complex numbers:
  inline JonesMatrix (const Complex &a00, const Complex & a01,
		      const Complex &a10, const Complex & a11);
  
  // Access to individual elements:
  inline Complex & operator       () (unsigned int i, unsigned int j);
  inline const Complex & operator () (unsigned int i, unsigned int j) const;

 private:

  // Default value = Identity matrix (Complex, 2x2)
  Complex m_a00{1.0}, m_a01{0.0};
  Complex m_a10{0.0}, m_a11{1.0};

};

inline std::ostream & operator << (std::ostream & o, const JonesMatrix & v);
inline JonesMatrix operator* (const JonesMatrix & m1, const JonesMatrix & m2);
inline JonesVector operator* (const JonesMatrix & m, const JonesVector & v);
#include "JONESCALC/JonesMatrix.icc"
#endif

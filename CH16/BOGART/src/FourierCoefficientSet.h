#ifndef _FOURIERCOEFFICIENTSET_H_
#define _FOURIERCOEFFICIENTSET_H_
#include <complex>
#include <vector>
#include <algorithm>
//---------------------------------------------------------
//
// This is a dedicated collection class to hold Fourier 
// coefficients in an expansion of a one-dimensional 
// function. 
//
//---------------------------------------------------------
class FourierCoefficientSet {
  
 public:

  // Constructor
  FourierCoefficientSet(unsigned int NMAX);

  // Accessors:

  // To elements:
  std::complex<double> & operator()(int i) ;
  const std::complex<double> & operator()(int i) const ;

  // To size:
  unsigned int size() const; 

  // To max n
 unsigned int nmax() const; 

 // Scale the coefficients:
 void operator *= (double a);

private:
  
  std::vector<std::complex<double> > c;
  
};

inline FourierCoefficientSet::FourierCoefficientSet(unsigned int NMAX) : 
  c(2*NMAX+1)
{
  std::fill(c.begin(),c.end(),0);
}

inline std::complex<double> &FourierCoefficientSet::operator() (int i) {
  return c[i+c.size()/2];
}

inline const std::complex<double> &FourierCoefficientSet::operator() (int i) const {
  return c[i+c.size()/2];
}

inline unsigned int FourierCoefficientSet::nmax() const {
  return c.size()/2;
}


inline unsigned int FourierCoefficientSet::size() const {
  return c.size();
}

inline void FourierCoefficientSet::operator *= (double d) {
  for (unsigned int i=0;i<c.size();i++) c[i]*=d;
}

#endif

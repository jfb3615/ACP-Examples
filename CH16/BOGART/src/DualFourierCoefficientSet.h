#ifndef _DUALFOURIERCOEFFICIENTSET_H_
#define _DUALFOURIERCOEFFICIENTSET_H_
#include <complex>
#include <vector>
#include <algorithm>
#include <stdexcept>
class DualFourierCoefficientSet {

 public:
  // Constructor
  DualFourierCoefficientSet(unsigned int NmaxX, unsigned int NmaxY);
  
  // Accessors:
  
  // To elements:
  std::complex<double> & operator()(int i, int j) ;
  const std::complex<double> & operator()(int i, int j) const ;
  
  // To size:
  unsigned int size() const; 
  
  // To max n
  unsigned int nmaxX() const; 
  unsigned int nmaxY() const; 
  
  // Scale the coefficients:
  void operator *= (double a);

 private:
  
  const unsigned int NMAXX;
  const unsigned int NMAXY;
  std::vector<std::complex<double>> c;
  
};

inline DualFourierCoefficientSet::DualFourierCoefficientSet(unsigned int NMaxX, 
							    unsigned int NMaxY) : 
				 NMAXX(NMaxX),NMAXY(NMaxY), c((2*NMAXX+1)*(2*NMAXY+1))
{
  std::fill(c.begin(),c.end(),0);
}

inline std::complex<double> &DualFourierCoefficientSet::operator() (int i, int j) {
  unsigned int I=i+NMAXX;
  unsigned int J=j+NMAXY;
  return c[(2*NMAXY+1)*J+I];
}

inline const std::complex<double> &DualFourierCoefficientSet::operator() (int i, int j) const {
  unsigned int I=i+NMAXX;
  unsigned int J=j+NMAXY;
  return c[(2*NMAXY+1)*J+I];
}

inline unsigned int DualFourierCoefficientSet::nmaxX() const {
  return NMAXX;
}

inline unsigned int DualFourierCoefficientSet::nmaxY() const {
  return NMAXY;
}


inline unsigned int DualFourierCoefficientSet::size() const {
  return c.size();
}

inline void DualFourierCoefficientSet::operator *= (double d) {
  for (unsigned int i=0;i<c.size();i++) c[i]*=d;
}

#endif

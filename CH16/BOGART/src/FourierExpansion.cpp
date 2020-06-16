#include "FourierExpansion.h"
namespace Genfun {

  FUNCTION_OBJECT_IMP(FourierExpansion)

  // The contructor
  FourierExpansion::FourierExpansion(const FourierCoefficientSet &coefficients, double scale):
    c(coefficients),scale(scale)
  {
  }
  
  // This form of the function call operator cannot be used:
  double FourierExpansion::operator() (double x) const {
    double value=0.0;
    static const std::complex<double> I(0,1);
    for (int i=-int(c.nmax());i<=int(c.nmax());i++) {
      value+=real(c(i)*sqrt(1.0/scale)*(exp(I*(i*M_PI*x/scale))));
    }
    return value;
  }
  
}

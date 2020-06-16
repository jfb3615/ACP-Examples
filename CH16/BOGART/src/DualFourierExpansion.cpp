#include "DualFourierExpansion.h"
namespace Genfun {

  FUNCTION_OBJECT_IMP(DualFourierExpansion)

  // The contructor
  DualFourierExpansion::DualFourierExpansion(const DualFourierCoefficientSet &coefficients, 
					     double scalex,
					     double scaley):
  c(coefficients),scaleX(scalex),scaleY(scaley)
  {
  }
  
  // This form of the function call operator cannot be used:
  double DualFourierExpansion::operator() (const Genfun::Argument & a) const {
    if (a.dimension()!=2) throw std::runtime_error("DualFourierExpansion:  dimension mismatch error");
    double value=0.0;
    const double &x=a[0], &y=a[1];
    double scale = sqrt(1.0/scaleX/scaleY);
    static const std::complex<double> I(0,1);
    std::complex<double> eX0=exp(-I*(M_PI*x/scaleX)),eX1=conj(eX0);
    std::complex<double> eY0=exp(-I*(M_PI*y/scaleY)),eY1=conj(eY0);
    std::complex<double> eX =pow(eX0,c.nmaxX());
    for (int i1=-int(c.nmaxX());i1<=int(c.nmaxX());i1++) {
      std::complex<double> eY =pow(eY0,c.nmaxY());
      for (int i2=-int(c.nmaxY());i2<=int(c.nmaxY());i2++) {
	value+=real(c(i1,i2)*scale*eX*eY); //(exp(I*(i1*M_PI*a[0]/scaleX+i2*M_PI*a[1]/scaleY))));
	eY*=eY1;
      }
      eX*=eX1;
    }
    return value;
  }
  
}

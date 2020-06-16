#include "BetheBlochFormula.h"
#include <cmath>

namespace Genfun {

  FUNCTION_OBJECT_IMP(BetheBloch)

  
  // Constructor:
  BetheBloch::BetheBloch (int Z,    
			  double A,    
			  double I,    
			  double M,    
			  int z,
			  double rho) :
  AbsFunction(), Z(Z),A(A),I(I),M(M),z(z),rho(rho)
  {
  }

  // Destructor:
  BetheBloch::~BetheBloch(){
  }

  // Copy Constructor:
  BetheBloch::BetheBloch(const BetheBloch & source):AbsFunction(),
						    Z(source.Z),
						    A(source.A),
						    I(source.I),
						    M(source.M),
						    z(source.z),
						    rho(source.rho)
  {
  }
  
  // Function Call
  double BetheBloch::operator() ( double betagamma) const {
    const double gamma=sqrt(1+betagamma*betagamma);
    const double beta = sqrt(betagamma*betagamma/(1+betagamma*betagamma));
    const double wmax=2.*me*betagamma*betagamma/
      (1.+2.*gamma*me/M + me*me/M/M);
    const double dEdX=K*z*z*Z/A/beta/beta*
      (0.5*log(2.*me*betagamma*betagamma*wmax/I/I)-beta*beta);
    return rho*dEdX;
  }

  
}

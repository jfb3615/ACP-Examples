#include "QatGenericFunctions/NormalDistribution.h"
#include "QatGenericFunctions/GaussQuadratureRule.h"
#include "QatGenericFunctions/GaussIntegrator.h"
// 
// Use 4pt (or more) Gauss Legendre Quadrature to compute the integral
// of a normal distribution from [-1,1].  
//
int main(int argc, char **argv) {
  
  using namespace Genfun;
  NormalDistribution f;
  
  GaussLegendreRule rule(argc==2 ? std::stoi(argv[1]):4);
  GaussIntegrator integrator(rule);

  double approx=integrator(f);
  double exact =erf(1.0/sqrt(2.0));
  std::cout << "Integral: " << integrator(f) <<  " error: " << approx-exact << std::endl;
  return 0;
}

#include "QatGenericFunctions/QuadratureRule.h"
#include "QatGenericFunctions/RombergIntegrator.h"
#include "QatGenericFunctions/SimpleIntegrator.h"
#include "QatGenericFunctions/Variable.h"
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <string>
#include <cmath>

int main () {

  using namespace Genfun;
  Variable X;
  GENFUNCTION f=2/(3+X)/(3+X);  
  RombergIntegrator integrator(-1,1, RombergIntegrator::CLOSED);
  integrator.setEpsilon(1.0E-12);
  std::cout << std::setprecision(10) << integrator(f)-1/2.0 << std::endl;
  std::cout << integrator.numFunctionCalls() << std::endl;
  // -------------------------------------------------:
  return 1;

}


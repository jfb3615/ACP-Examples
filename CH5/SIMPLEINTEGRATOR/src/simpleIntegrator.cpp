#include "QatGenericFunctions/QuadratureRule.h"
#include "QatGenericFunctions/SimpleIntegrator.h"
#include "QatGenericFunctions/Sin.h"
#include "QatGenericFunctions/Sqrt.h"
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <string>
#include <cmath>

int main () {

  using namespace Genfun;
  GENFUNCTION f=Sqrt();
  
  TrapezoidRule rule;
  SimpleIntegrator integrator(0,1, rule,16); 
 
  std::cout << std::setprecision(10) << integrator(f)-2.0/3.0 << std::endl;

  return 1;

}


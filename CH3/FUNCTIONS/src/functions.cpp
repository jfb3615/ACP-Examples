#include "QatGenericFunctions/F1D.h"
#include "QatGenericFunctions/Sin.h"
#include "QatGenericFunctions/Exp.h"
#include <iostream>
#include <cmath>
int main (int , char **) {

  using namespace Genfun;

  Sin sin;
  Exp exp;
  GENFUNCTION f=sin/exp;
  
  for (int i=0;i<100;i++) {
    double x=i/10.0;
    double y=f(x);
    std::cout << x << " " << y << std::endl;
  }

  return 1;

}


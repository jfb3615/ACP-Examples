#include <iostream>
#include <cmath>
#include <gsl/gsl_ieee_utils.h>
int main() {
  double pi=M_PI;
  gsl_ieee_printf_double(&pi);
  std::cout << std::endl;
  return 1;
}

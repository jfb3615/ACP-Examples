#include <iostream>
#include <cmath>
//----------------------------------------------------------------
// 
// Obtain the value of PI from the series expansion of arcsin
//
// Use the c math library functions:
//
// exp
// lgamma (log of the gamma function.
// pow    (power function)
//
//----------------------------------------------------------------
double F (unsigned int n) {
  return exp(lgamma(2*n+1)-2*lgamma(n+1))/pow(4.0,n)/(2*n+1.0);
}

//
// An arcsin function using a truncated expansion.
//
double arcsin(double x, int trunc) {
  double result=0.0;
  for (int i=0;i<trunc;i++) {
    result += F(i)*pow(x,2*i+1);
  }
  return result;
}
//
// Main routine compute PI with various truncations.
//
int main () {

  
  for (unsigned int i=0; i<=1000;i++) {
    double result= 6.0*arcsin(0.5,i)-M_PI;
    std::cout << i << " " << result << std::endl;
    if (!std::isfinite(result)) break;
  }
  
  return 0;
}

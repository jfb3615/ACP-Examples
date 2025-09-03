#include <complex>
#include <iostream>
#include <cmath>
#include <iomanip>
#include <limits>
#include <numbers>
// 
// threePi is a small program to obtain pi in three
// different ways:
//
int main () {

  // 1. Just print out the macro; (std::numbers::pi requires C++20)
  std::cout << std::setprecision(16) << std::numbers::pi << std::endl;

  // 2. From the area of inner inscribed polygons.  Start
  //    with a hexagon and subdivide the arc into smaller
  //    pieces. Liu Hui(n=3072, 265 AD)  
  //    Zu Chongzhi (n=12288, 480 AD) 

  std::complex x0(1.0);
  std::complex x1(1.0/2.,sqrt(3.0)/2.0);
  unsigned int nsides=6;
  while (nsides < std::numeric_limits<int>::max()) {

    double lside = abs(x1-x0);
    double approx= nsides*lside/2.0;
    std::cout << "Sides " << nsides << "; approx=" << std::setprecision(16) << approx << std::endl;

    x1=(x1+x0)/std::abs(x1+x0);
    nsides *=2; 
  }
  
  // 3. Take the logarithm of the imaginary number -1:
  std::cout << std::setprecision(16) << imag(std::log(std::complex(-1.0))) << std::endl;
  return 0;
}

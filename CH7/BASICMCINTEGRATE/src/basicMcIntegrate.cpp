#include <cstdlib>
#include <iostream>
#include <string>
#include <random>
int main (int, char **) {

  std::mt19937 e;
  const unsigned int N=10000;

  {
    std::exponential_distribution<double> eDist;
    double sum=0,sum2=0;
    for (unsigned int i=0;i<N;i++) {
      double x=eDist(e);
      double f=x*x;
      sum  +=   f;
      sum2 += f*f;
    }
    double fBar=sum/N,f2Bar=sum2/N, sigma=sqrt((f2Bar-fBar*fBar)/N);
    std::cout << "Integral is " << fBar  << "+-" << sigma << std::endl;
  }

  {
    std::gamma_distribution<double> g2Dist(2);
    double sum=0,sum2=0;
    for (unsigned int i=0;i<N;i++) {
      double x=g2Dist(e);
      double f=x;
      sum  +=   f;
      sum2 += f*f;
    }
    double fBar=sum/N,f2Bar=sum2/N, sigma=sqrt((f2Bar-fBar*fBar)/N);
    std::cout << "Integral is " << fBar  << "+-" << sigma << std::endl;
  }

  // -------------------------------------------------:
  return 1;

}


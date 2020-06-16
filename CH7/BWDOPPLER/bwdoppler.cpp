#include <random>
int main (int argc, char * * argv) {
  std::mt19937 e;
  std::normal_distribution<double> n;
  std::cauchy_distribution<double> u;


  for (int i=0;i<100000;i++) double y=(u(e)+n(e));

}


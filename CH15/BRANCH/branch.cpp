#include <iostream>
#include <random>
using namespace std;



enum DecayMode {BBBAR, TAUPAIR, MUONPAIR, CHARMPAIR, GLUONPAIR, GAMMAPAIR, ZGAMMA, WPAIR, ZPAIR, OTHER};


int main() {

  // Initialize random number generators:
  std::random_device dev;
  std::mt19937 engine(dev());

  // Initialize the generator:
  std::discrete_distribution<int> BR = { 5.809E-1,
					 6.256E-2,
					 2.171E-4,
					 2.884E-2,
					 8.180E-2,
					 2.270E-3,
					 1.541E-3,
					 2.152E-1,
					 2.641E-2};

  // Draw a random decay mode and print it out
  DecayMode mode=(DecayMode) BR(engine);
  std::cout << mode << std::endl;

  // Done
  return 0;
}

#include "JONESCALC/JonesVector.h"
#include "JONESCALC/JonesMatrix.h"
#include <iostream>
#include <string>
int main (int argc, char ** argv) {

  JonesVector v0(JonesVector::Horizontal);
  JonesMatrix PH(JonesMatrix::FastHorizontal);
  JonesMatrix DG(JonesMatrix::Diagonal);
  JonesMatrix PV(JonesMatrix::FastVertical);

  // Make a circular polarizer:
  JonesMatrix CP=PH*DG*PV;
  JonesVector v1=CP*v0;

  std::cout << "Initial= " << v0 << std::endl;
  std::cout << "Filter= " << std::endl;
  std::cout << CP << std::endl;
  std::cout << "Final intensity= " << v1.magsq() << std::endl;
  std::cout << "Final polarization= " << v1*(1/sqrt(v1.magsq())) << std::endl;

  return 1;
}

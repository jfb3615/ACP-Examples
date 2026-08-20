#include "LegendreLib.h"
#include <string>
#include <vector>

int main(int argc, char* argv[]) {
  
  // Parse the input, retreive the degree of the Legendre polynomial. 
  int l=parseInput(argc, argv);
  
  // Create the canvas 81 x 41 (should be odd numbers)
  std::vector<std::string> canvas=getCanvas(81,41);

  // Plot the Legendre polynomial:
  plotLegendre(canvas, l);
  
  return 0;
}

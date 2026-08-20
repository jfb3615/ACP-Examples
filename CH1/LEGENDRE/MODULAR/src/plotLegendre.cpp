#include "LegendreLib.h"
#include <iostream>
#include <cmath> 
void plotLegendre(std::vector<std::string> & canvas , int l) {
  int height=canvas.size(), width=canvas[0].size();
  // Do the mathematical part, evaluating the function:
  std::vector<double> values(width);
  for (int col = 0; col < width; ++col) {
    double x = -1.0 + 2.0 * col / (width - 1);
    double y = std::legendre(l, x);
    values[col] = y;
  }
  // Plot curve
  for (int col = 0; col < width; ++col) {
    int row =  (int) std::round( (1.0 - values[col]) / 2.0 * (height-1)); 
    canvas[row][col] = '*';
  }
   
  std::cout << "\nLegendre Polynomial P_" << l << "(x)\n" << std::endl;
  for (const auto& line : canvas)  std::cout << line << '\n';
}


#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <sstream>

int parseInput(int argc, char *argv[]) {
  std::string usage = std::string ("usage: ") + argv[0] + " <l> ";

  // Parse the command line:
  if (argc != 2)  {
    std::cerr << usage << std::endl;
    exit(1);
  }
  
  int l{0};
  std::istringstream stream(argv[1]);
  if (!(stream >> l)) {
    std::cerr << usage << std::endl;
    exit(2);
  } else if (l < 0) {
    std::cerr << "l must be non-negative.\n";
    exit (3);
  }
  else {
    return l;
  }
  
}

std::vector<std::string> getCanvas(int width, int height) {

  std::vector<std::string> canvas(height, std::string(width, ' '));
  
  // Draw horizontal axis (y = 0)
  int x_axis_row =  (int) std::round( (height-1)/2.0);
  if (0 <= x_axis_row && x_axis_row < height) {
    for (int c = 0; c < width; ++c) canvas[x_axis_row][c] = '-';
  }
  
  // Draw vertical axis (x = 0)
  int y_axis_col = width / 2;
  for (int r = 0; r < height; ++r)   canvas[r][y_axis_col] = '|';

  // Draw origin
  canvas[x_axis_row][y_axis_col] = '+';
  
  return canvas;
}

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

int main(int argc, char* argv[]) {
  
  // Parse the input, retreive the degree of the Legendre polynomial. 
  int l=parseInput(argc, argv);
  
  // Create the canvas 81 x 41 (should be odd numbers)
  std::vector<std::string> canvas=getCanvas(81,41);

  // Plot the Legendre polynomial:
  plotLegendre(canvas, l);
  
  return 0;
}

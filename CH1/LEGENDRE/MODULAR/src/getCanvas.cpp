#include "LegendreLib.h"
#include <cmath>
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

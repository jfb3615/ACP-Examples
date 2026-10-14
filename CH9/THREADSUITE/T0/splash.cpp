#include "render.h"
#include <iostream>
// Main Routine: 
int main() {
  
  std::cout << "Splash!! executes in single threaded mode [Return to continue]..." << std::endl;
  std::cin.get();
  std::cout << "Generating..." << std::endl;

    
  static PixelArray pixels{};
  for (int i=0;i<16;i++) createImage(pixels);  
  render(pixels);
  return 0;
}

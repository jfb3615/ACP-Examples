#include "render.h"
#include <iostream>
#include <time.h>
// Main Routine: 
int main() {
  
  std::cout << "Splash!! executes in single threaded mode [Return to continue]..." << std::endl;
  std::cin.get();
  std::cout << "Generating..." << std::endl;
  
  time_t start = time(NULL); 
  static PixelArray pixels{};
  for (int i=0;i<16;i++) createImage(&pixels);  
  time_t end = time(NULL); 
  std::cout << "Real time:  " << difftime(end,start) << " seconds" << std::endl;
  
  render(pixels);
  return 0;
}

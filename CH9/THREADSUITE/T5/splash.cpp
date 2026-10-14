#include "render.h"
#include <iostream>
#include <thread>
#include <vector>
// Main Routine: 
int main() {
  
  std::cout << "Splash!! MT, std::unique_lock protecting the critical region [Return to continue]..." << std::endl;
  std::cin.get();
  std::cout << "Generating..." << std::endl;

    
  int  NCPU=std::thread::hardware_concurrency();
  std::cout <<  "On this machine there are " << NCPU << " CPUs" << std::endl;

  static PixelArray pixels{};
  
  {
    std::vector<std::jthread> threadVector;
    for (int i=0;i<NCPU;i++) threadVector.emplace_back(createImage,&pixels);
  }
  
  render(pixels);
  return 0;
}

#include "render.h"
#include <iostream>
#include <thread>
#include <vector>
#include <time.h>
// Main Routine: 
int main() {
  
  std::cout << "Splash!! MT, with mutexes on the critical region [Return to continue]..." << std::endl;
  std::cin.get();
  std::cout << "Generating..." << std::endl;

    
  int  NCPU=std::thread::hardware_concurrency();
  std::cout <<  "On this machine there are " << NCPU << " CPUs" << std::endl;

  time_t start=time(NULL);
  static PixelArray pixels{};
  
  {
    std::vector<std::jthread> threadVector;
    for (int i=0;i<NCPU;i++) threadVector.emplace_back(createImage,&pixels);
  }
  time_t end=time(NULL);
  std::cout << "Real time:  " << difftime(end,start) << " seconds" << std::endl;

  render(pixels);
  return 0;
}

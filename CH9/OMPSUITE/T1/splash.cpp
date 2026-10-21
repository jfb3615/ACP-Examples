#include "SplashGenerator.h"
#include "render.h"
#include <thread>
#include <iostream>
#include <sstream>
#include <string>
#include "omp.h"
int main(int argc, char **argv) {

  // Define here the usage statement. 
  std::string usage{argv[0]};
  usage += " <slowDownNumber> ";

  // Get number of CPUS
  int NPROCESSORS=std::thread::hardware_concurrency();
 


  // Get the slowdown number from the command line: 
  int slowDownNumber{0};
  if (argc==2) {
    std::istringstream stream(argv[1]);
    if (!(stream >> slowDownNumber)) {
      std::cerr << "usage: " << usage << std::endl;
      exit(1);
    }
  }

  using SplashGenPtr = std::unique_ptr<SplashGenerator>;
  std::vector<SplashGenPtr> splashGenerator;
  for (int i=0;i<NPROCESSORS;i++) {
    splashGenerator.emplace_back(std::make_unique<SplashGenerator>(slowDownNumber));
  }
  
#pragma omp parallel for
  for (int i=0;i<NPROCESSORS;i++) {
    splashGenerator[i]->compute();
  }
#pragma omp parallel
#pragma omp critical
  {
    std::cout << "HELLO from thread number" << omp_get_thread_num() <<  "/" << omp_get_num_threads() << std::endl;
  }
#pragma omp barrier
#pragma omp master
  {
      std::cout << "HELLO from me the master!!" << std::endl;
  }
  
  PixelArray pixels;
  
  for (SplashGenPtr & s : splashGenerator) {
    const PixelArray *layer=s->harvest();
    for ( int i=0;i<layer->size();i++) {
      if ((*layer)[i]!=0) pixels[i]=(*layer)[i];
    }
  }
  render(pixels);
  return 0;
}

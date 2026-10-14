#include "SplashGenerator.h"
#include "render.h"
#include <thread>
#include <iostream>
#include <sstream>
#include <string>

int main(int argc, char **argv) {

  // Define here the usage statement. 
  std::string usage{argv[0]};
  usage += " <slowDownNumber> ";

  // Get number of CPUS
  int NPROCESSORS=std::thread::hardware_concurrency();
 


  // Get the slowdown number from the command line: 
  int slowDownNumber{1000};
  if (argc==2) {
    std::istringstream stream(argv[1]);
    if (!(stream >> slowDownNumber)) {
      std::cerr << "usage: " << usage << std::endl;
      exit(1);
    }
  }

  using SplashGenPtr = std::unique_ptr<SplashGenerator>;
  std::vector<SplashGenPtr> splashGenerator;
  {
    std::vector<std::jthread> threadVector;
    for (int i=0;i<NPROCESSORS;i++) {
      splashGenerator.emplace_back(std::make_unique<SplashGenerator>(slowDownNumber));
      threadVector.emplace_back(&SplashGenerator::compute,splashGenerator.back().get());
    }
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

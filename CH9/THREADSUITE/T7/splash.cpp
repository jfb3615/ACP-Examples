#include "render.h"
#include <cstdint>
#include <random>
#include <mutex>
#include <algorithm>
#include <atomic>
thread_local std::random_device dev;
thread_local std::mt19937       engine(dev());

// random distributions for choosing
thread_local std::uniform_int_distribution                 xPos{0,kImageWidth};  // x-position of splash
thread_local std::uniform_int_distribution                 yPos{0,kImageHeight}; // y-position of splash
thread_local std::uniform_int_distribution<std::uint32_t>  colorChoice{0,255};   // color of splash (r,g,b
thread_local std::gamma_distribution                       width{30.0,6.0};      // width of splash;
thread_local std::uniform_real_distribution                flat;                 // pixel probability

PixelArray pixels{};
std::mutex mtx;

// This routine writes onto the canvas:
void createImage()
{
  {
    std::unique_lock lock(mtx);
    
    // Select the center;
    int x=xPos(engine);      // choose the z position;
    int y=yPos(engine);      // choose the y position
    double w=width(engine);  // choose the width;
    std::uint32_t color =qRgb(colorChoice(engine),  // choose
			      colorChoice(engine),  // the
			      colorChoice(engine)); // color
    // Color each pixel:
    for (int row = 0; row < kImageHeight; ++row) {
      for (int column = 0; column < kImageWidth; ++column) {
	double distance2=std::pow(row-x,2)+std::pow(column-y,2);
	double probability = exp(-distance2/w/w);
	// Mill around for a while: 10 iterations=1.5 sec/splay
	//for (int i=0;i<1000;i++) flat(engine); 
	// .... done
	if (flat(engine) < probability) {
	  const int index = row * kImageWidth + column;
	  pixels[index] = color;
	}
      }
    }
  }
  for (int i=0;i<100;i++) std::reverse(pixels.begin(),pixels.end());
}



#include <thread>
#include <iostream>

int main() {

  int NPROCESSORS=std::thread::hardware_concurrency();
  std::cout << "On this computer there are " << NPROCESSORS << " CPUs" << std::endl;

  {
    std::vector<std::jthread> threadVector;
    for (int i=0;i<NPROCESSORS;i++) {
      threadVector.emplace_back(std::jthread(createImage));
    }
  }
  
  
  render(pixels);
  return 0;
}

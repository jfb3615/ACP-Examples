#include "render.h"
#include <random>
#include <mutex>

// This routine writes the splash onto the Canvas:
std::mutex mtx;
void createImage(PixelArray *pixels)
{
  std::random_device dev;
  std::mt19937       engine(dev());
  
  // random distributions for choosing size, position, and color of the splash:
  std::uniform_int_distribution      xPos{0,kImageWidth};  // x-position of splash
  std::uniform_int_distribution      yPos{0,kImageHeight}; // y-position of splash
  std::uniform_int_distribution<int> colorChoice{0,255};   // color of splash (r,g,b
  std::gamma_distribution            width{30.0,6.0};      // width of splash;
  std::uniform_real_distribution     flat;                 // pixel probability
  


  // Select the center;
  int x=xPos(engine);                  // choose the z position;
  int y=yPos(engine);                  // choose the y position
  double w=width(engine);              // choose the width;
  Triplet color{colorChoice(engine),   // choose
          colorChoice(engine),   // the
          colorChoice(engine)};  // color
  // Color the pixels:
  mtx.lock();
  for (int row = 0; row < kImageHeight; ++row) {
    for (int column = 0; column < kImageWidth; ++column) {
      double distance2=std::pow(row-x,2)+std::pow(column-y,2);
      double probability = std::exp(-distance2/w/w);
      for (int i=0;i<1000;i++) flat(engine);// use CPU 
       if (flat(engine) < probability) {
         const int index = row * kImageWidth + column;
         (*pixels)[index] = color;
      }
    }
  }
  mtx.unlock();
}

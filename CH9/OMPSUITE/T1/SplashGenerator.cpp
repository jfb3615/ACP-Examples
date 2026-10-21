#include "SplashGenerator.h"

SplashGenerator::SplashGenerator(int slow_Down_Length):
  slowDownLength(slow_Down_Length)
{
}

void SplashGenerator::compute() {
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
      double probability = std::exp(-distance2/w/w);
      // Mill around for a while: 10 iterations=1.5 sec/splay
      for (int i=0;i<slowDownLength;i++) flat(engine); 
      // .... done
      if (flat(engine) < probability) {
	const int index = row * kImageWidth + column;
	pixels[index] = color;
      }
    }
  }
}

const PixelArray *SplashGenerator::harvest() const {
  return &pixels;
}

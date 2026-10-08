#include "render.h"
#include <cstdint>
#include <random>


std::random_device dev;
std::mt19937       engine(dev());

// random distributions for choosing
std::uniform_int_distribution                 xPos{0,kImageWidth};  // x-position of splash
std::uniform_int_distribution                 yPos{0,kImageHeight}; // y-position of splash
std::uniform_int_distribution<std::uint32_t>  colorChoice{0,255};   // color of splash (r,g,b
std::gamma_distribution                       width{30.0,6.0};      // width of splash;
std::uniform_real_distribution                flat;                 // pixel probability

PixelArray pixels{};

// This routine writes onto the canvas:
void createImage()
{

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
      // Mill around for a while: 1K iterations=1.5 sec/splay
      // for (int i=0;i<1000;i++) flat(engine); 
      // .... done
      if (flat(engine) < probability) {
	const int index = row * kImageWidth + column;
	pixels[index] = color;
      }
    }
  }
}




int main() {

  for (int i=0;i<16;i++) createImage();
  render(pixels);
  return 0;
}

#include "render.h"
#include <iostream>
// Main Routine:
int main() {
  PixelArray pixels;
  for (int i=0;i<16;i++) {
    createImage(pixels);
  }
  render(pixels);
  return 0;
}

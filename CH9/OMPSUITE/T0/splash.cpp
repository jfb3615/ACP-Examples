#include "render.h"
#include <iostream>
#include "omp.h"
// Main Routine:
int main() {
  PixelArray pixels;
#pragma omp parallel for
  for (int i=0;i<16;i++) {
    {
      createImage(pixels);
    }
  }
  render(pixels);
  return 0;
}

#pragma once
#include "render.h"
#include <random>
class SplashGenerator {

 public:

  // Constructor
  SplashGenerator(int slowDownLength=0);
  
  // Carry out the computations
  void compute();

  // Harvest the result
  const PixelArray *harvest() const;

 private:

  // Turn off copy and assignment
  
  SplashGenerator(const SplashGenerator &)               = delete;
  SplashGenerator & operator = (const SplashGenerator &) = delete;


  
  std::random_device dev;
  std::mt19937       engine{dev()};
  
  // random distributions for choosing
  std::uniform_int_distribution<>                 xPos{0,kImageWidth};  // x-position of splash
  std::uniform_int_distribution<>                 yPos{0,kImageHeight}; // y-position of splash
  std::uniform_int_distribution<std::uint32_t>    colorChoice{0,255};   // color of splash (r,g,b
  std::gamma_distribution<>                       width{30.0,6.0};      // width of splash;
  std::uniform_real_distribution<>                flat;                 // pixel probability
  
  // Array of pixels to be generated;
  PixelArray pixels{};
  int slowDownLength{0};
};

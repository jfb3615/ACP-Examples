#pragma once
#include "render.h"

class SplashGenerator {

 public:

  // Constructor
  SplashGenerator();
  
  // Carry out the computations
  void compute();

  // Harvest the result
  const PixelArray *harvest() const;

 private:

  // Turn off copy and assignment
  
  SplashGenerator(const SplashGenerator &)               = delete;
  SplashGenerator & operator = (const SplashGenerator &) = delete;

};

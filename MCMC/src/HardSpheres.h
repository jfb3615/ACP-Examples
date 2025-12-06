#ifndef _HARDSPHERES_MODEL_H_
#define _HARDSPHERES_MODEL_H_
#include "Eigen/Dense"
#include <random>
#include <numbers>
#include <cmath>
#include <optional>
#include "AbsModel.h"
using EngineType=std::mt19937;

class HardSpheres:public AbsModel {

public:


  

  // Constructor:
  HardSpheres(unsigned int nParticles, double mass=1.0, double radius=0.10, double T=0.9); 
  
  // Take a step in time. 
  virtual bool takeStep(double deltaT) override final;

  double getRadius() const {return a;}
  
 private:

  static constexpr double L{1.0};  //dimension of box
  double T{0.9};                   //initial gass energy
  double a{0.10};
  std::random_device rd{};
  EngineType engine{rd()};

};
#endif

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
  HardSpheres(unsigned int nParticles, double mass=1.0, double radius=0.10, double Energy=1000.0); 
  
  // Take a step in time. 
  virtual void takeStep(double deltaT) override final;

  // Get energies:
  double getKineticEnergy() const override final;
  double getPotentialEnergy() const override final;
  double getWorkOnEnvironment() const override final;
  double getTotalEnergy() const override final;

  double getRadius() const {return a;}
  
 private:

  class Collision;
  std::optional<Collision> detectCollision(unsigned int i, unsigned int j)  const;
  
  static constexpr double L{1.0};  //dimension of box
  double E0{1000.0};              //initial gass energy
  double a{0.10};
  std::random_device rd{};
  EngineType engine{rd()};

};
#endif

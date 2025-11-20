#ifndef _PERIODICIDEAL_MODEL_H_
#define _PERIODICIDEAL_MODEL_H_
#include "Eigen/Dense"
#include <random>
#include <numbers>
#include <cmath>
#include "AbsModel.h"
using EngineType=std::mt19937;

class PeriodicIdeal:public AbsModel {

public:


  

  // Constructor:
  PeriodicIdeal(unsigned int nParticles, double mass=1.0, double Energy=1000.0); 
  
  // Take a step in time. 
  virtual void takeStep(double deltaT) override final;

  // Get energies:
  double getKineticEnergy() const override final;
  double getPotentialEnergy() const override final;
  double getWorkOnEnvironment() const override final;
  double getTotalEnergy() const override final;
    
 private:

  static constexpr double L{1.0};  //dimension of box
  double E0=1000.0;         //initial gass energy
 
  std::random_device rd{};
  EngineType engine{rd()};

};
#endif

#ifndef _IDEALGAS_MODEL_H_
#define _IDEALGAS_MODEL_H_
#include "Eigen/Dense"
#include <random>
#include <numbers>
#include <cmath>
#include "AbsModel.h"
using EngineType=std::mt19937;

class IdealGasModel:public AbsModel {

public:

  // Constructor:
  IdealGasModel(unsigned int nParticles,
		double mass=1.0,
		double pistonMass=100.0,
		double energy=1000.0); 
  
  // Take a step in time. 
  virtual void takeStep(double deltaT) override final;

  // Get the piston position()
  double getPistonZ() const { return zp;}

  // Get energies:
  double getKineticEnergy() const override final;
  double getPotentialEnergy() const override final;
  double getWorkOnEnvironment() const override final;
  double getTotalEnergy() const override final;
    
 private:
  
  static constexpr double a2{0.0005};         //radius squared of molecular collision
  static constexpr double r{1.0};            //Radius of cylinder
  static constexpr double r2=1.0;            //Its square

  const double pmass{100.0};                 //mass of piston
  double E0=1000.0;                          //initial gass energy

  double zp=2.0;                             // z position of piston

  double vpist    {0.0};                     // velocity of piston
  mutable double work     {0.0};             // work done on piston
  mutable double kinetic  {0.0};
  std::random_device rd{};
  EngineType engine{rd()};
  std::uniform_real_distribution<double> uniform;
  std::normal_distribution<double> gauss;

  // 
  void computeState() const; 
  mutable bool stale{true};

};
#endif


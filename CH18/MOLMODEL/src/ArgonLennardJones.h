#ifndef _ARGONLENNARDJONES_MODEL_H_
#define _ARGONLENNARDJONES_MODEL_H_
#include "Eigen/Dense"
#include <random>
#include <numbers>
#include <cmath>
#include "AbsModel.h"
using EngineType=std::mt19937;

class ArgonLennardJones:public AbsModel {

public:


  

  // Constructor:
  ArgonLennardJones(unsigned int nParticles, double mass=1.0, double radius=0.10, double interactionStrength=1.0, double Energy=1000.0, bool Lattice=false); 
  
  // Take a step in time. 
  virtual void takeStep(double deltaT) override final;

  // Get energies:
  double getKineticEnergy() const override final;
  double getPotentialEnergy() const override final;
  double getWorkOnEnvironment() const override final;
  double getTotalEnergy() const override final;

  // Get the "radius" (sigma of Lennard-Jones potential")
  double getRadius() const {return a;}
  double getInteractionStrength() const {return interactionStrength;}
  
 private:

  static constexpr double L{1.0};  //dimension of box
  double a{0.10};
  double interactionStrength{1.0};
  double E0=1000.0;         //initial gass energy
  bool   lattice{false};    //latice (vs random) initialization
  std::random_device rd{};
  EngineType engine{rd()};

  // 
  void computeState() const; 
  mutable bool   stale{true};
  mutable double kinetic{0};
  mutable double potential{0};
  
};
#endif

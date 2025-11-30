#ifndef _ARGONLENNARDJONES_MODEL_H_
#define _ARGONLENNARDJONES_MODEL_H_
#include "Eigen/Dense"
#include <random>
#include <numbers>
#include <cmath>
#include <optional>
#include "AbsModel.h"
using EngineType=std::mt19937;

class ArgonLennardJones:public AbsModel {

public:


  

  // Constructor:
  ArgonLennardJones(unsigned int nParticles, double mass=1.0, double radius=0.10, double ePs=0.8, double T=0.9); 
  
  // Take a step in time. 
  virtual bool takeStep(double deltaT) override final;

  double getRadius() const {return a;}
  
 private:

  double getPotential(unsigned int i, const Eigen::VectorXd & X) const;
  double getKinetic(const Eigen::VectorXd & V) const;
  
  static constexpr double L{1.0};  //dimension of box
  double ePs{0.8};                 //constant in Lennard-jones potential
  double T{0.9};                   //initial gass energy
  double a{0.10};
  std::random_device rd{};
  EngineType engine{rd()};

};
#endif

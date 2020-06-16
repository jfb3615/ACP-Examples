#ifndef _SIMULATOR_H_
#define _SIMULATOR_H_
#include "QatGenericFunctions/AbsFunction.h"
#include <vector>
#include <random>
using namespace Genfun;

class Simulator {

 public:
  
  // Constructor:
  Simulator(GENFUNCTION U, double x, unsigned int NSTEPS);

  // Destructor:
  ~Simulator();

  // Simulate:
  void simulate();

  // Harvest:
  double position(unsigned int t) const;
  double weight(unsigned int t) const;
  
  // Length of the simulation:
  size_t length() const; 
  
private:

  GENFUNCTION                                     U;
  std::vector<double>                             pos;
  std::vector<double>                             wgt;
  static std::random_device                       rndDevice;
  std::mt19937                                    engine;
  std::bernoulli_distribution                     flip;
  
  };
#endif

#ifndef _ISINGMODEL_H_
#define _ISINGMODEL_H_

class IsingModel {
 public:

  // Constructor
  IsingModel(unsigned int NX=400, unsigned int NY=400, double tau=5.0);

  // Destructor
  ~IsingModel();

  // Get dimensions of this model:
  unsigned int NX() const;
  unsigned int NY() const;

  // Access & modification of temperature
  double & tau();
  const double & tau() const;

  // Internal energy
  int U() const;
 
  // Magnetization
  int M() const;

  // Goto next state
  void next();
  
  // Get i, j, delta;
  void lastStep(unsigned int & i, unsigned int &j, int &delta);

  // Get the state of each dipole:
  bool isUp(unsigned int i, unsigned int j);

 private:

  IsingModel(const IsingModel &) = delete;
  IsingModel & operator =(const IsingModel &) = delete;

  class Clockwork;
  Clockwork *c;
};
#include "IsingModel.icc"
#endif


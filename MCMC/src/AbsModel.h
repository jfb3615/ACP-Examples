#ifndef _ABSMODEL_H_
#define _ABSMODEL_H_
#include "Eigen/Dense"
class AbsModel {

public:

  // Constructor:
  AbsModel(unsigned int nParticles, double mass); 

  // Destructor
  virtual ~AbsModel()=default;

  // Get the number of particles:
  unsigned int getNumParticles() const;

  // Get the particle mass
  double getParticleMass() const;
  
  // Take a step in time
  virtual bool takeStep(double deltaT)=0;

  // Get the positions
  const Eigen::Vector3d & getPosition(unsigned int i) const;

  // Get the velocities
  const Eigen::Vector3d & getVelocity(unsigned int i) const;

 private:

  AbsModel(const AbsModel &)               =delete;
  AbsModel & operator = (const AbsModel &) =delete;
  
  const unsigned int nParticles{};         // # of molecules
  const double       mass{};               // molecule mass

protected:
  
  std::vector<Eigen::Vector3d> x,v;  // phase space coords

};

#include "AbsModel.tpp"
#endif

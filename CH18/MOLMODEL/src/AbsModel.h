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
  virtual void takeStep(double deltaT)=0;

  // Get current time, sum of all the time steps.
  double getTime() const;

  // Get the positions
  const Eigen::Vector3d & getPosition(unsigned int i) const;

  // Get the velocities
  const Eigen::Vector3d & getVelocity(unsigned int i) const;

  // Get energies:
  virtual double getKineticEnergy()     const=0;
  virtual double getPotentialEnergy()   const=0;
  virtual double getWorkOnEnvironment() const=0;
  virtual double getTotalEnergy()       const=0;  // kinetic+potential
    
 private:

  AbsModel(const AbsModel &)               =delete;
  AbsModel & operator = (const AbsModel &) =delete;
  
  const unsigned int nParticles{};         // # of molecules
  const double       mass{};               // molecule mass

protected:
  
  double   time{0.0};                // time since t0
  std::vector<Eigen::Vector3d> x,v;  // phase space coords


};

#include "AbsModel.tpp"
#endif

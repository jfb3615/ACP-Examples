#include "PeriodicIdeal.h"
#include <stdexcept>
#include <iostream>
using namespace std;
using namespace Eigen;

PeriodicIdeal::PeriodicIdeal(unsigned int nParticles, double mass, double energy):AbsModel(nParticles, mass),E0(energy)  {

  // Initialize the positions and velocities of all molecules:
  std::uniform_real_distribution uniform(-L/2.0,L/2.0);
  std::normal_distribution       gauss;
  
  
  for (unsigned int i=0;i<getNumParticles();i++) {
    x[i]=Vector3d(uniform(engine),uniform(engine),uniform(engine));
    v[i]=Vector3d(gauss(engine),gauss(engine),gauss(engine));
  }

  double egas=0;
  double amass=getParticleMass();
  for(unsigned int i=0;i<getNumParticles();i++) { 
    double v2=v[i].squaredNorm();
    egas=egas+0.5*amass*v2;
  }
  
  double fFactor=sqrt(E0/egas);
  for(unsigned int i=0;i<getNumParticles();i++) v[i]*=fFactor;
  
}

void PeriodicIdeal::takeStep(double deltaT) {
  using namespace std;
  time+=deltaT;
  
  for (unsigned int i=0;i<getNumParticles();i++) {
    
    x[i]+=(v[i]*deltaT);
    while (x[i].x()<-L/2.0) x[i].x()+=L;
    while (x[i].y()<-L/2.0) x[i].y()+=L;
    while (x[i].z()<-L/2.0) x[i].z()+=L;
    while (x[i].x()>L/2.0) x[i].x()-=L;
    while (x[i].y()>L/2.0) x[i].y()-=L;
    while (x[i].z()>L/2.0) x[i].z()-=L;
  }
     
}
  
  

double PeriodicIdeal::getKineticEnergy() const {
  return E0;
}

double PeriodicIdeal::getPotentialEnergy() const {
  return 0;
}

double PeriodicIdeal::getTotalEnergy() const {
  return E0;
}

double PeriodicIdeal::getWorkOnEnvironment() const {
  return 0.0;
}

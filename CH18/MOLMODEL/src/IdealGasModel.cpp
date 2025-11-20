#include "IdealGasModel.h"
#include <stdexcept>
#include <iostream>
using namespace std;
using namespace Eigen;

IdealGasModel::IdealGasModel(unsigned int nParticles, double mass, double pistonMass, double energy):AbsModel(nParticles, mass),pmass(pistonMass),E0(energy)  {
  // Initialize the positions and velocities of all molecules:
  for (unsigned int i=0;i<getNumParticles();i++) {
    x[i]=Vector3d((2.0*uniform(engine)-1.0)*r,(2.0*uniform(engine)-1.0)*r,zp*uniform(engine));
    while ( sqrt(x[i](0)*x[i](0)+x[i](1)*x[i](1)) > 1) {
      x[i]=Vector3d((2.0*uniform(engine)-1.0)*r,(2.0*uniform(engine)-1.0)*r,zp*uniform(engine));
      
    }
    
    v[i]=Vector3d(gauss(engine),gauss(engine),gauss(engine));

  }

  double egas=0;
  double amass=getParticleMass();
  for(unsigned int i=0;i<getNumParticles();i++)
    { 
      double v2=v[i].squaredNorm();
      egas=egas+0.5*amass*v2;
    }

  double fFactor=sqrt(E0/egas);
  for(unsigned int i=0;i<getNumParticles();i++) v[i]*=fFactor;

}

void IdealGasModel::takeStep(double deltaT) {
  using namespace std;
  stale=true;
  double amass=getParticleMass();
  time+=deltaT;
  // 10
  {
    zp=zp+vpist*deltaT;
    //
    // Every 100th time slice, print out some information about
    // the energy of piston and gas of molecules:
    //
      
    for (unsigned int i=0;i<getNumParticles();i++)//6
      {
	// Propagate:
	auto perp=[](const Vector3d & a, const Vector3d & b) {
	  return a.x()*b.x()+a.y()*b.y();
	};

	Vector3d xPrime=x[i]+(v[i]*deltaT);
	double   xP=perp(xPrime,xPrime);
	// 
	// Collision with side walls?
	//
	if (xP > r2) {
	  double v2=perp(v[i],v[i]);
	  double b=2.0*perp(v[i],x[i])/v2;
	  double c=(perp(x[i],x[i])-r2)/v2;
	  double tp1=0.5*(-b+sqrt(b*b-4*c));
	  Vector3d xImpact=x[i]+tp1*v[i];
	  double distanceTo=(xImpact-x[i]).norm();
	  double distanceLeft=v[i].norm()*deltaT-distanceTo;
	  Vector3d nHat=Vector3d(xImpact.x(),xImpact.y(),0).normalized();
	  v[i] -= (2.0*nHat*(v[i].dot(nHat)));
	  x[i] = xImpact+v[i]*distanceLeft;
	}
	else {
	  x[i]=xPrime;
	}
	
	if (x[i].z()<0) v[i](2)=-v[i].z();
	
	
	// 
	// Collision with the Piston?
	//
	if (x[i].z()>zp)
	  {
	    double vzp=(2.0*pmass*vpist+(amass-pmass)*v[i].z())/(amass+pmass);
	    vpist=(2.0*amass*v[i].z()+(pmass-amass)*vpist)/(amass+pmass);
	    v[i](2)=vzp;
	  }
	
      }//6
  }//10
}

void IdealGasModel::computeState() const {
  if (!stale) return;
  kinetic=0;
  double amass=getParticleMass();
  for(unsigned int i=0;i<getNumParticles();i++) { 
    double v2=v[i].squaredNorm();
    kinetic=kinetic+0.5*amass*v2;
  }
  work=0.5*pmass*vpist*vpist;
}

double IdealGasModel::getKineticEnergy() const {
  if (stale) computeState();
  return kinetic;
}

double IdealGasModel::getPotentialEnergy() const {
  return 0;
}

double IdealGasModel::getTotalEnergy() const {
  if (stale) computeState();
  return kinetic;
}

double IdealGasModel::getWorkOnEnvironment() const {
  if (stale) computeState();
  return work;
}

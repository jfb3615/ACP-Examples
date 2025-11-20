#include "MolecularModel.h"
#include <stdexcept>
#include <iostream>
using namespace std;
using namespace Eigen;

MolecularModel::MolecularModel(unsigned int nParticles, double mass, double pistonMass):AbsModel(nParticles, mass),pmass(pistonMass)  {
  // Initialize the positions and velocities of all molecules:
  for (unsigned int i=0;i<getNumParticles();i++) {
    x[i]=Vector3d((2.0*myrand(engine)-1.0)*r,(2.0*myrand(engine)-1.0)*r,zp*myrand(engine));
    while ( sqrt(x[i](0)*x[i](0)+x[i](1)*x[i](1)) > 1) {
      x[i]=Vector3d((2.0*myrand(engine)-1.0)*r,(2.0*myrand(engine)-1.0)*r,zp*myrand(engine));
      
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

void MolecularModel::takeStep(double deltaT) {
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
	x[i]+=(v[i]*deltaT);
	
	// 
	// Collision with side walls?
	//
	if ( sqrt(x[i](0)*x[i](0)+x[i](1)*x[i](1)) > r2) 
	  {
	    Vector3d nHat=Vector3d(x[i].x(),x[i].y(),0).normalized();
	    v[i] -= (2.0*nHat*(v[i].dot(nHat)));
	    if (!isfinite(v[i].squaredNorm())) {
	      throw runtime_error("infinite quantity");
	    }
	  }
	
	if (x[i].z()<0) v[i](2)=-v[i].z();
	if (!isfinite(v[i].squaredNorm())) {
	  throw runtime_error("infinite quantity");
	}
	
	
	// 
	// Collision with the Piston?
	//
	if (x[i].z()>zp)
	  {
	    double E0=0.5*amass*v[i].squaredNorm() + 0.5*pmass*vpist*vpist;
	    double vzp=(2.0*pmass*vpist+(amass-pmass)*v[i].z())/(amass+pmass);
	    vpist=(2.0*amass*v[i].z()+(pmass-amass)*vpist)/(amass+pmass);
	    v[i](2)=vzp;
	    if (!isfinite(vpist)) {
	      throw runtime_error("infinite quantity");
	    }
	    if (!isfinite(v[i].squaredNorm())) {
	      throw runtime_error("infinite quantity");
	    }
	    double E1=0.5*amass*v[i].squaredNorm() + 0.5*pmass*vpist*vpist;
	    if (E0-E1>0.00001) {
	      cout << "Energy nonconservation" << endl;
	    }

	  }
	
      }//6
    

    for(unsigned int i=0;i<getNumParticles()-1;i++)//50
      {
	for (unsigned int j=i+1;j<getNumParticles();j++)//51
	  {
	    // 
	    // Collision with another molecule?
	    //
	    double tst = (x[i]-x[j]).squaredNorm();
	    if (tst<a2)
	      {
		Vector3d C=v[i]+v[j],S=v[i]-v[j];
		double s = S.norm();
		double ct=2.0*myrand(engine)-1;
		double st= sqrt(max(0.0,(1+ct)*(1-ct)));
		double phi=2.*M_PI*myrand(engine);
		S=Vector3d(s*st*cos(phi),s*st*sin(phi),s*ct);
		v[i]=0.5*(C+S);
		v[j]=0.5*(C-S);
		if (!isfinite(v[i].squaredNorm())) {
		  throw runtime_error("infinite quantity");
		}
		if (!isfinite(v[j].squaredNorm())) {
		  throw runtime_error("infinite quantity");
		}
	      }	       
	  }//51
      }//50
  }//10
}

void MolecularModel::computeState() const {
  if (!stale) return;
  kinetic=0;
  double amass=getParticleMass();
  for(unsigned int i=0;i<getNumParticles();i++) { 
    double v2=v[i].squaredNorm();
    kinetic=kinetic+0.5*amass*v2;
  }
  work=0.5*pmass*vpist*vpist;
}

double MolecularModel::getKineticEnergy() const {
  if (stale) computeState();
  return kinetic;
}

double MolecularModel::getPotentialEnergy() const {
  return 0;
}

double MolecularModel::getTotalEnergy() const {
  if (stale) computeState();
  return kinetic;
}

double MolecularModel::getWorkOnEnvironment() const {
  if (stale) computeState();
  return kinetic;
}

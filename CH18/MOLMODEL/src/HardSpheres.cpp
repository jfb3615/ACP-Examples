#include "HardSpheres.h"
#include <stdexcept>
#include <iostream>
#include <set>
using namespace std;
using namespace Eigen;

class HardSpheres::Collision {
public:
  unsigned int i{0};
  unsigned int j{0};
  double cTime{0.0};
  bool operator < (const Collision & right) const  {return cTime < right.cTime; }
};


std::optional<HardSpheres::Collision> HardSpheres::detectCollision(unsigned int i, unsigned int j) const {

  Vector3d x1=x[i],x2=x[j], X=x2-x1;
  Vector3d v1=v[i],v2=v[j], V=v2-v1;

  
  // Apply the minimum image condition:
  for (unsigned int d=0;d<3;d++) {
    if (X[d] > L/2.0)  X[d]-=L;
    if (X[d] < -L/2.0) X[d]+=L;
  }

  if (X.dot(V)<0) {
      double B= 2.0*V.dot(X);
      double A= V.dot(V);
      double C=X.dot(X)-4*a*a;
      double D=B*B-4.0*A*C;
      if (D>0) {
	double d=sqrt(D);
	double t = std::min((-B-d)/2.0/A , (-B+d)/2.0/A);
	return Collision{i,j,t};
      }
      else {
	return {};
      }
  }
 
  return {};
}

HardSpheres::HardSpheres(unsigned int nParticles, double mass, double radius, double energy):AbsModel(nParticles, mass),E0(energy),a(radius)  {

  //
  std::cout << "HELLO from HardSpheres.  Seeking initial conditions" << std::endl;
  // Initialize the positions and velocities of all molecules:
  std::uniform_real_distribution uniform(-L/2.0,L/2.0);
  std::normal_distribution       gauss;
  
  for (unsigned int i=0;i<getNumParticles();i++) {
    while (1) {
      Vector3d X=Vector3d(uniform(engine),uniform(engine),uniform(engine));
      bool ok{true};
      for (unsigned int j=0;j<getNumParticles();j++) {

	// Apply the minimum image condition:
	VectorXd Xdiff=x[j]-X;
	for (unsigned int d=0;d<3;d++) {
	  if (Xdiff[d] > L/2.0)  Xdiff[d]-=L;
	  if (Xdiff[d] < -L/2.0) Xdiff[d]+=L;
	}

	if (Xdiff.squaredNorm()<4*a*a) {
	  ok=false;
	  break;
	}
      }
      if (ok) {
	x[i]=X;
	break;
      }
    }
  }


  
  for (unsigned int i=0;i<getNumParticles();i++) {
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


void HardSpheres::takeStep(double deltaT) {
  using namespace std;
  std::set<Collision> upcomingCollision;
  
  for (unsigned int i=0;i<getNumParticles();i++) {
    for (unsigned int j=i+1;j<getNumParticles();j++) {
      std::optional<Collision> collision= detectCollision(i,j);
      if (collision) {
	upcomingCollision.insert(*collision);
      }
    }
  }
  // Introduce an intermediate step if a collision occurs.
  auto c=upcomingCollision.begin();
  if (c!=upcomingCollision.end()) {
    const Collision & collision=*c;
    if (collision.cTime<deltaT) {
      std::cout << "Collide \a" << std::endl;
      unsigned int i=collision.i, j=collision.j; 
      x[i]+=(v[i]*collision.cTime);
      while (x[i].x()<-L/2.0) x[i].x()+=L;
      while (x[i].y()<-L/2.0) x[i].y()+=L;
      while (x[i].z()<-L/2.0) x[i].z()+=L;
      while (x[i].x()>L/2.0) x[i].x()-=L;
      while (x[i].y()>L/2.0) x[i].y()-=L;
      while (x[i].z()>L/2.0) x[i].z()-=L;
      Vector3d v1=v[i],v2=v[j], dv=(v2+v1)/2.0;
      Vector3d v1P=v1-dv,v2P=v2-dv;
      v[i]=dv-v1P;
      v[j]=dv-v2P;
      deltaT-=collision.cTime;
      time+=deltaT;
      takeStep(deltaT);
      return;
    }
  }
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
  return;

}
  
  

double HardSpheres::getKineticEnergy() const {
  return E0;
}

double HardSpheres::getPotentialEnergy() const {
  return 0;
}

double HardSpheres::getTotalEnergy() const {
  return E0;
}

double HardSpheres::getWorkOnEnvironment() const {
  return 0.0;
}

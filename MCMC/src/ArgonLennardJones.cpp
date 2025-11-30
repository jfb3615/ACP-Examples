#include "ArgonLennardJones.h"
using namespace Eigen;

ArgonLennardJones::ArgonLennardJones(unsigned int nParticles, double mass, double radius, double ePs, double T):AbsModel(nParticles, mass),ePs(ePs),T(T),a(radius)  {

  // Initialize the positions and velocities of all molecules:
  std::uniform_real_distribution uniform(-L/2.0,L/2.0);
  std::normal_distribution       gauss;

  unsigned int c{0};
  unsigned int nPer=std::cbrt(getNumParticles());
  double delta= 1.0/nPer;
  for (unsigned int i=0;i<nPer;i++) {
    double xi=-L/2.0+i*delta+ delta/2.0;
    for (unsigned int j=0;j<nPer;j++) {
      double y=-L/2.0+j*delta + delta/2.0;
      for (unsigned int k=0;k<nPer;k++) {
	double z=-L/2.0+k*delta+delta/2.0;
	Vector3d X(xi,y,z);
	x[c++]=X;
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

 double E=1.5*getNumParticles()*T;
  double fFactor=sqrt(E/egas);
  for(unsigned int i=0;i<getNumParticles();i++) v[i]*=fFactor;

  
}

double ArgonLennardJones::getKinetic(const VectorXd & V) const {

 
  return 0.5*getParticleMass()*V.squaredNorm();
}

double ArgonLennardJones::getPotential(unsigned int i, const VectorXd & X) const {
  double result{0};
  for (unsigned int j=0;j<getNumParticles();j++) {
    if (j!=i) {
      // Apply the minimum image condition:
      VectorXd Xdiff=x[j]-X;
      for (unsigned int d=0;d<3;d++) {
	if (Xdiff[d] > L/2.0)  Xdiff[d]-=L;
	if (Xdiff[d] < -L/2.0) Xdiff[d]+=L;
      }
      double Rdiff2=Xdiff.squaredNorm();
      double U=4*ePs*(std::pow(a*a/Rdiff2,6)-std::pow(a*a/Rdiff2,3));
      result += U;
    }
  }
  return result;
}


bool ArgonLennardJones::takeStep(double deltaT) {
  std::uniform_int_distribution<unsigned int>  select(0,getNumParticles()-1);
  std::uniform_real_distribution<double>       toKeepOrNot(0,1);
  std::normal_distribution      gauss(0.0,deltaT*0.1);
  unsigned int i=select(engine);
  Vector3d X=x[i], V=v[i];

  double myPotential0=getPotential(i,X);
  double myKinetic0  =getKinetic(V);
  
  X.x()+=gauss(engine);
  X.y()+=gauss(engine);
  X.z()+=gauss(engine);

  V.x()+=gauss(engine);
  V.y()+=gauss(engine);
  V.z()+=gauss(engine);

  
  while (X.x()<-L/2.0) X.x()+=L;
  while (X.y()<-L/2.0) X.y()+=L;
  while (X.z()<-L/2.0) X.z()+=L;
  while (X.x()>L/2.0)  X.x()-=L;
  while (X.y()>L/2.0)  X.y()-=L;
  while (X.z()>L/2.0)  X.z()-=L;

  double myPotential1=getPotential(i,X);
  double myKinetic1  =getKinetic(V);
  double deltaE= (myPotential1+myKinetic1)-(myPotential0+myKinetic0);
 
  bool ok = deltaE<0 ? true : toKeepOrNot(engine) < std::exp(-deltaE/T);
  
  
  if (ok)  {
    x[i]=X;
    v[i]=V;
  }
  return ok;
}
  
  


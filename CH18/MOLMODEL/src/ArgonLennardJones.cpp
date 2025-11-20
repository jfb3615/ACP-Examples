#include "ArgonLennardJones.h"
#include <stdexcept>
#include <iostream>
#include <thread>
#include <map>
using namespace std;
using namespace Eigen;



class PartialCalculator {

public:
  
  PartialCalculator(const AbsModel & model,
		    std::vector<std::vector<Vector3d>>     & FIJ,
		    std::vector<std::vector<unsigned int>> & IIJ,
		    double epsilon, double sigma, unsigned int iProcessor): model(model),FIJ(FIJ),IIJ(IIJ),ePs(epsilon),sigma(sigma), iProcessor(iProcessor) {} 
  void compute();
  
private:

  const AbsModel& model;
  std::vector<std::vector<Vector3d>>           & FIJ;
  std::vector<std::vector<unsigned int>>       & IIJ; // for processor assignment
  const double ePs{};
  const double sigma{};
  const unsigned int iProcessor{};
  static constexpr double L{1.0};

};

void PartialCalculator::compute() {
  unsigned int N=model.getNumParticles();
  for (unsigned int i=0;i<N;i++) {
    for (unsigned int j=i+1;j<N;j++) {
      if (IIJ[i][j]==iProcessor) {
	const VectorXd & x1=model.getPosition(i);
	const VectorXd & x2=model.getPosition(j);
	Vector3d x=x1-x2;
	// Apply the minimum image condition:
	for (unsigned int d=0;d<3;d++) {
	  while (x[d] > L/2.0)  x[d]-=L;
	  while (x[d] < -L/2.0) x[d]+=L;
	}
	Vector3d xHat=x.normalized();
	double   r=x.norm();
	double   F=24.0*ePs/sigma*(2*std::pow(sigma/r,13)-std::pow(sigma/r,7));
	Vector3d f=F*xHat;
	FIJ[i][j]=f;
	FIJ[j][i]=-f;
      }
    }
  }
}


class ForceCalculator {
  
public:
  
  ForceCalculator (const AbsModel &model, double epsilon, double sigma);
  
  void compute();

  const Vector3d & getForce (unsigned int i) {return FI[i];}
  
private:

  ForceCalculator(const ForceCalculator & )    = delete;
  ForceCalculator & operator=(const ForceCalculator &  ) = delete;
  
  const AbsModel & model;
  std::vector<std::vector<Vector3d>>           FIJ;
  std::vector<std::vector<unsigned int>>       IIJ; // for processor assignment
  std::vector<Vector3d>                        FI;  // final answer here. 
  double ePs{1.0};
  double sigma{0.01};
  static constexpr double L{1.0};
};



ForceCalculator::ForceCalculator (const AbsModel &model, double epsilon, double sigma) :model(model),ePs(epsilon),sigma(sigma){
  // Allocate the array:
  const unsigned int N=model.getNumParticles();
  for (unsigned int i=0;i<N;i++) {
    FIJ.emplace_back(std::vector<Vector3d>(N));
    for (unsigned int j=0;j<N;j++) FIJ[i][j]=Vector3d::Zero();
  }
  
  for (unsigned int i=0;i<N;i++) IIJ.emplace_back(std::vector<unsigned int>(N));
  unsigned int NPROCESSORS=std::thread::hardware_concurrency();
  unsigned int c{0};
  for (unsigned int i=0;i<N;i++) {
    for (unsigned int j=i+1;j<N;j++) {
      IIJ[i][j]=c;
      c++;
      c%=NPROCESSORS;
     
    }
  }
}



void  ForceCalculator::compute() {
  unsigned int N=model.getNumParticles();
  unsigned int NPROCESSORS=std::thread::hardware_concurrency();

  std::vector<PartialCalculator> workers;
  for (unsigned int iProcessor=0;iProcessor<NPROCESSORS;iProcessor++) {
    workers.emplace_back(PartialCalculator(model,FIJ,IIJ,ePs, sigma, iProcessor));
  }
  
  std::vector<std::thread> threaders;
  for (unsigned int iProcessor=0;iProcessor<NPROCESSORS;iProcessor++) {
    threaders.emplace_back(&PartialCalculator::compute, workers[iProcessor]);
  }

  
  for (unsigned int iProcessor=0;iProcessor<NPROCESSORS;iProcessor++) {
    threaders[iProcessor].join();
  }
  
  for (unsigned int i=0;i<N;i++) {
    FI.emplace_back(Vector3d::Zero());
    for (unsigned int j=0;j<N;j++) {
      if (i!=j) FI[i]+=FIJ[i][j];
    }
  }
    
}
  
ArgonLennardJones::ArgonLennardJones(unsigned int nParticles, double mass,double radius, double interactionStrength,  double energy, bool lattice):AbsModel(nParticles, mass),a(radius), interactionStrength(interactionStrength), E0(energy),lattice(lattice)  {

  // Initialize the positions and velocities of all molecules:
  std::uniform_real_distribution uniform(-L/2.0,L/2.0);
  std::normal_distribution       gauss;
  
  
  if (!lattice) { // Do random positions
    for (unsigned int i=0;i<getNumParticles();i++) {
      x[i]=Vector3d(uniform(engine),uniform(engine),uniform(engine));
    }
  }
  else {
    std::map<unsigned int, unsigned int> cuberoots{{8,2},{27,3},{64,4},{125,5},{216,6},{343,7},{512,8},{729,9},{1000,10}};
    unsigned int N=getNumParticles();
    unsigned int N0=cuberoots[N];
    std::cout << N<<","<<N0 << std::endl;
    if (N0==0) throw std::runtime_error("Choose N Perfect Cube please. 8,27,64,125, 216, 343, 512, 729, or 1000");
    double Delta = L/N0;
    unsigned int c{0};
    for (unsigned int l=0;l<N0;l++) {
      double xx=-L/2.0+l*Delta+0.005*gauss(engine); 
      for (unsigned int j=0;j<N0;j++) {
	double yy=-L/2.0+j*Delta+0.005*gauss(engine); 
	for (unsigned int k=0;k<N0;k++) {
	  double zz=-L/2.0+k*Delta+0.005*gauss(engine);
	  x[c++]=Vector3d(xx,yy,zz);
	}
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

void ArgonLennardJones::takeStep(double deltaT) {
  using namespace std;
  const double amass=getParticleMass();
  stale=true;
  
  time+=deltaT;

  if (0) { // Classic Verlet algorithm
    // First step of the Verlet method:
    for (unsigned int i=0;i<getNumParticles();i++) {
      x[i]+=(v[i]*deltaT/2.0);
    }
    // Second step
    ForceCalculator calculator(*this,getInteractionStrength(),getRadius());
    calculator.compute();     
    for (unsigned int i=0;i<getNumParticles();i++) {
      const Vector3d & force=calculator.getForce(i);
      v[i]+=(deltaT/amass)*force;
    }
    // Third step step of the Verlet method:
    for (unsigned int i=0;i<getNumParticles();i++) {
      x[i]+=(v[i]*deltaT/2.0);
    }
  }
  else { // Velocity Verlet method:
    // First step of velocity Verlet method:
    {
      ForceCalculator calculator(*this,getInteractionStrength(),getRadius());
      calculator.compute();     
      for (unsigned int i=0;i<getNumParticles();i++) {
	const Vector3d & force=calculator.getForce(i);
	v[i]+=(deltaT/amass/2.0)*force;
      }
    }
    // Second step
    for (unsigned int i=0;i<getNumParticles();i++) {
      x[i]+=(v[i]*deltaT);
    }
    // Third step of velocity Verlet method.
    {
      ForceCalculator calculator(*this,getInteractionStrength(),getRadius());
      calculator.compute();     
      for (unsigned int i=0;i<getNumParticles();i++) {
	const Vector3d & force=calculator.getForce(i);
	v[i]+=(deltaT/amass/2.0)*force;
      }
    }
  }

  
  for (unsigned int i=0;i<getNumParticles();i++) {
    while (x[i].x()<-L/2.0) x[i].x()+=L;
    while (x[i].y()<-L/2.0) x[i].y()+=L;
    while (x[i].z()<-L/2.0) x[i].z()+=L;
    while (x[i].x()>L/2.0) x[i].x()-=L;
    while (x[i].y()>L/2.0) x[i].y()-=L;
    while (x[i].z()>L/2.0) x[i].z()-=L;
  }
}
  
  

double ArgonLennardJones::getKineticEnergy() const {
  if (stale) computeState();
  return kinetic;
}

double ArgonLennardJones::getPotentialEnergy() const {
  if (stale) computeState();
  return potential;
}

double ArgonLennardJones::getTotalEnergy() const {
  if (stale) computeState();
  return getPotentialEnergy()+getKineticEnergy();
}

double ArgonLennardJones::getWorkOnEnvironment() const {
  return 0.0;
}


void ArgonLennardJones::computeState() const {
  if (!stale) return;
  kinetic=0;
  potential=0;
  double amass=getParticleMass();
  for(unsigned int i=0;i<getNumParticles();i++) {
    double v2=v[i].squaredNorm();
    kinetic=kinetic+0.5*amass*v2;
  }
  for(unsigned int i=0;i<getNumParticles();i++) {
    for(unsigned int j=i+1;j<getNumParticles();j++) {
      double rij=(x[i]-x[j]).norm();
      potential+= 4*interactionStrength*(std::pow(a/rij,12)-std::pow(a/rij,6));
    }
  }
  
  stale=false;
  std::cout << "Kinetic energy = " << kinetic <<  " Potential " << potential <<  std::endl;
}

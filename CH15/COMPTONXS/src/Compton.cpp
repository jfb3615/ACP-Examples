#include "Compton.h"
#include "Spacetime/FourVector.h"
#include "Spacetime/ThreeVector.h"
#include "Spacetime/DiracSpinor.h"
#include "Spacetime/LorentzTransformation.h"
#include "Eigen/Dense"
#include <complex>
#include <functional>
typedef std::complex<double> Complex;
// Constructor:
Compton::Compton(double k0, bool isPolarized) :
  k0(k0) ,
  beta(1.0/(1.0+m/k0)),
  tCM(ThreeVector(0,0,-atanh(beta))),
  k(tCM*FourVector(k0,0,0,k0)),
  p(tCM*FourVector(m,0,0,0)),
  u{tCM*DiracSpinor::U(up,m), tCM*DiracSpinor::U(down,m)},
  isPolarized(isPolarized)
{}

const double Compton::alpha=1/137.0;
const double Compton::m=0.511;
const ThreeVector Compton::nullVector(0,0,0);
const ThreeVector Compton::zHat(0,0,1);
const FourVector Compton::eps[2] =  {FourVector(0,1,0,0),FourVector(0,0,1,0)};
const Spinor <> Compton::up={1,0};
const Spinor<> Compton::down={0,1};
const Eigen::Matrix4cd Compton::M=Compton::m*Eigen::Matrix4cd::Identity();


//
// Compute the cross section as a function of theta and phi, all
// quantities here refer to the lab frame.
//
double Compton::sigma(double cosThetaLab, double phiLab) {

  //-------------------------------------------Define some constants-----------------------------------------------------
  static const Complex I(0,1);
  static const double e=sqrt(4*M_PI*alpha);

  //--------------------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------------------
  // We calculate the scattering angle in the C.M. frame:
  double cosThetaCM=(cosThetaLab-beta)/(1-beta*cosThetaLab);
  double sinThetaCM = sqrt((1+cosThetaCM)*(1-cosThetaCM));
  double phiCM=phiLab;
  //--------------------------------------------------------------------------------------------------------------------------------
    
  //--------------------------------------------------------------------------------------------------------------------------------
  ThreeVector nHat(sinThetaCM*cos(phiCM), sinThetaCM*sin(phiCM), cosThetaCM);
  LorentzTransformation rot(nullVector, zHat.cross(nHat).normalized()*acos(cosThetaCM));
  FourVector kPrime=rot*k;// In case: pOut=rot*p;
  //--------------------------------------------------------------------------------------------------------------------------------
  
  //--------------------------------------------------------------------------------------------------------------------------------
  // Compute the Feynman diagrams:
  double sumAmplitudeSquared=0.0;
  size_t lim=isPolarized ? 1:2;
  for (size_t i0=0;i0<lim;i0++) {  // sum (?) over incoming photon polarization
    const FourVector & epsIn=eps[i0];
    for (size_t i1=0;i1<2;i1++) {// sum over incoming electron polarization
      const DiracSpinor::U & uIn=u[i1];
      for (size_t o0=0;o0<2;o0++) {  // sum over outgoing photon polarization
	FourVector epsPrime=rot*eps[o0];
	for (size_t o1=0;o1<2;o1++) {// sum over outgoing electron polarization
	  DiracSpinor::U uPrime=rot*u[o1];
	  // all objects constructed.  Now compute the amplitude(s).
	  { 
	    using namespace SpecialOperators;

	    // Compute the S-channel Feynman diagram:
	    Complex MS = e*e*((bar(uPrime)*(slash(epsPrime.conjugate())*(slash(p+k)+M)/((p+k).squaredNorm()-m*m)*slash(epsIn))*uIn)(0));
	    // Compute the T-channel Feynman diagram:
	    Complex MT = e*e*((bar(uPrime)*(slash(epsIn)*(slash(p-kPrime)+M)/((p-kPrime).squaredNorm()-m*m)*slash(epsPrime.conjugate()))*uIn)(0));
	    
	    double amplitudeSquared=norm(MT+MS);
	    sumAmplitudeSquared+=amplitudeSquared;
	  }
	}
      }
    }
  }
  //--------------------------------------------------------------------------------------------------------------------------------
  
  double den= isPolarized ? 2.0:4.0;
  double aveSumAmpSquared=sumAmplitudeSquared/den;
  double ks=1./(1./k0+1./m*(1.-cosThetaLab));
  double re=alpha/m;
  double phaseSpaceFactor=pow(ks/(8.0*M_PI*k0*m),2);
  // answer in units of re^2:

  return phaseSpaceFactor *aveSumAmpSquared/(re*re);

}

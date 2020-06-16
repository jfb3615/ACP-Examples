#include "KleinNishina.h"
#include "Spacetime/FourVector.h"
#include "Spacetime/ThreeVector.h"
#include "Spacetime/DiracSpinor.h"
#include "Spacetime/LorentzTransformation.h"
#include "Eigen/Dense"
#include <complex>
#include <functional>
typedef std::complex<double> Complex;
// Constructor:
KleinNishina::KleinNishina(double kLab, bool isPolarized) :
  m(0.511),
  kLab(kLab) ,
  isPolarized(isPolarized)
{
  // Set up the initial state.
}


//
// Compute the cross section as a function of theta and phi, all
// quantities here refer to the lab frame.
//
double KleinNishina::sigma(double cosTheta, double phi) {

  double kPrime=1.0/(1.0/kLab + 1.0/m*(1.0-cosTheta));
  if (isPolarized) {
        return kPrime*kPrime/(kLab*kLab)*(kLab/kPrime + kPrime/kLab +(-2.0 + 2.0*cosTheta*cosTheta)*cos(phi)*cos(phi))/2.0;
  }
  else {
    return kPrime*kPrime/(kLab*kLab)*(kLab/kPrime + kPrime/kLab -1.0 + cosTheta*cosTheta)/2.0;
  }
}

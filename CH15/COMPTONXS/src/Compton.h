#ifndef _Compton_h_
#define _Compton_h_
#include "Spacetime/FourVector.h"
#include "Spacetime/LorentzTransformation.h"
#include "Spacetime/DiracSpinor.h"

class Compton {

 public:

  // Constructor:
  Compton(double k0, bool isPolarized=false);

  // Compute the cross section in units of the classical electron radius squared ( rₑ²)
  // where  rₑ = 1/m Mev⁻¹ and  m=0.511 MeV.
  
  double sigma(double cosThetaLab, double phiLab=0.0);
  
 private:

  // The initial state:
  const double k0;                                     // photon energy, lab frame.
  const double beta;                                  // velocity of cm frame.
  const LorentzTransformation tCM;           // to center-of-mass frame
  const FourVector k;                                 // photon four-vector in C.M.
  const FourVector p;                                 // electron four-vector in C.M.
  const DiracSpinor::U u[2];                       // Dirac spinors 

  
  // Constants:
  static const double m;                             // electron mass
  static const double alpha;                        // fine structure constant
  static const Spinor <> up;                       // spinor= (1,0)^T
  static const Spinor <> down;                  // spinor= (0,1)^T
  static const FourVector eps[2];                // X and Y polarization vectors
  static const ThreeVector nullVector;        // The null vector
  static const ThreeVector zHat;                // Unit vector along z
  static const Eigen::Matrix4cd M;             // m*Identity matrix:

  // Flag:
  const bool           isPolarized;                  // switch on polarization
  

};
#endif

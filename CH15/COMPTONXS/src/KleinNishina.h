#ifndef _KleinNishina_h_
#define _KleinNishina_h_
class KleinNishina {

 public:

  // Constructor:
  KleinNishina(double kLab, bool isPolarized=false);

  // Compute the cross section in units of the classical electron radius squared ( rₑ²)
  // where  rₑ = 1/m Mev⁻¹ and  m=0.511 MeV.
  
  double sigma(double cosThetaLab, double phiLab=0.0);
  
 private:

  // The initial state:
  const double m;                                       // electron mass
  const double kLab;                                  // photon energy, lab frame.
  const bool     isPolarized;                        // switch on polarization
};
#endif

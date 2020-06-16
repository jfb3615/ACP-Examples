#include "QatGenericFunctions/AbsFunction.h"
//------------------------------------------------------------------------------------------------------------//
//                                                                                                                         //
//                                                                                                                         //
//                                                                                                                         //
//-----------------------------------------------------------------------------------------------------------//
#ifndef _BetheBloch_h_
#define _BetheBloch_h_
namespace Genfun {
class BetheBloch:public AbsFunction {

  FUNCTION_OBJECT_DEF(BetheBloch)

    public:
    
  // Constructor:
  BetheBloch(int    Z,    // atomic number of absorber
	     double A,    // atomic mass of absorber (g/mol);
	     double I,    // mean exitation energy of absorber (MeV) 
	     double M,    // mass of incoming (MeV)
	     int  z=1,    // charge of incoming
	     double rho=1.0); // density of absorber); 

  // Destructor:
  virtual ~BetheBloch();
  
  // Copy Constructor:
  BetheBloch (const BetheBloch & right);
  
  // Retrieve function value (here for function of one variable)
  virtual double operator ()(double argument) const;
  virtual double operator ()(const Argument & a) const {return operator() (a[0]);}
  
 private:
  
  const BetheBloch & operator=(const BetheBloch &right)=delete;
  
  const int    Z;   // atomic number of target material
  const double A;   // atomic mass of target material, g/mol
  const double I;   // mean exitation energy (MEV!!!) 
  const double M;   // mass of incoming
  const int    z;   // charge of incoming.  Will default to 1;
  const double rho; // density of absorber g/cm^3. Will default to 1.0
  static constexpr double K  =0.307075;    // Mev mol^-1 cm^2
  static constexpr double me =0.510998928; // Mass of the electron,MeV.
};
}
#endif

//--------------------------------------------------------------------------//
//                                                                          //
// Class Psi2Hydrogen.  The hydrogen wavefunction^2 as a function of r      //
// theta, and phi.                                                          //
//                                                                          //
// Joe Boudreau April 2002                                                  //
//                                                                          //
//--------------------------------------------------------------------------//
#ifndef Psi2Hydrogen_h
#define Psi2Hydrogen_h 1
#include "QatGenericFunctions/AbsFunction.h"
#include "QatGenericFunctions/Parameter.h"

namespace Genfun {

  class Psi2Hydrogen : public AbsFunction  {
  
    FUNCTION_OBJECT_DEF(Psi2Hydrogen)

 public:

    // Constructor
    Psi2Hydrogen(unsigned int n, unsigned int l, unsigned int m);

    // Copy constructor
    Psi2Hydrogen(const Psi2Hydrogen &right);
  
    // Destructor
    virtual ~Psi2Hydrogen();
  
    // Retreive function value
    virtual double operator ()(double argument) const;   // Gives an error
    virtual double operator ()(const Argument & a) const;// Must use this one
  
    // Get the integer variable n
    unsigned int n() const;

    // Get the integer variable l
    unsigned int l() const;

    // Get the integer variable m
    unsigned int m() const;

  private:

    // It is illegal to assign an adjustable constant
    const Psi2Hydrogen & operator=(const Psi2Hydrogen &right);

    // Here is the constant n
    unsigned int _n;

    // Here is the constant l 
    unsigned int _l;

    // Here is the constant m
    unsigned int _m;

    // Here is the "work function"
    const AbsFunction *_function;

    // This function is needed in all constructors:
    void create(); 
  };

}

#include "Psi2Hydrogen.icc"

#endif


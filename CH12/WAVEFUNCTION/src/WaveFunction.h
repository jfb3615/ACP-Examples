// ----------------------------------------------------------//
//                                                           //
// This is an example of how to write your own Qat Generic   //
// Function. It consist of a class WaveFunction implementing //
// the wave function^2 of a particle in a stepwise potential.// 
// The function is governed by a set of steps, each one      //
// specifying the start-of-step x and the "index of          //
// refraction" sqrt(1-V/E), which may be complex in a        //
// classically forbidden region.                             //
//                                                           //
// It corresponds to an exercise (#7, ch3) but illustrates:  //
//           * A solution not requiring global variables.    //
//           * How to extend QatGenericFunctions             //
//           * Polymorphism in General.                      //
//                                                           //
// Here you have the header file                             //
//                                                           //
// ----------------------------------------------------------// 
#ifndef _WAVEFUNCTION_H_
#define _WAVEFUNCTION_H_
#include "QatGenericFunctions/AbsFunction.h"
#include "Eigen/Dense"
#include <complex>
namespace Genfun {
  class WaveFunction:public AbsFunction {

    // This macro declares a number of methods required
    // by the base class but which we generate here
    // automatically in the macro so you do not need to
    // know about those..See the .cpp file where a 
    // corresponding macro is invoked. 
    FUNCTION_OBJECT_DEF(WaveFunction)

  public:

    // A nested structure to specify the steps.
    struct Step {
      double                    x;
      std::complex<double>      n;
    };

    // The contructor
    WaveFunction(const std::vector<Step> & steps);

    // Function call operator returns the |wavefunction|^2
    // which is of course always real.
    double operator() (double x) const;

    // Function call operator, argument form. Should fail if dimension > 1. 
    double operator() (const Argument & a) const {
      if (a.dimension()!=1) throw std::runtime_error("WaveFunction:  dimensionality mismatch");
      return operator()(a[0]);
    }

  private:

    Eigen::VectorXcd         sVector;
    const std::vector<Step>  step;

  };
}
#endif

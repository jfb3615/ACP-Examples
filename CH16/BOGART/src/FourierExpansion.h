//-----------------------------------------------------------//
//                                                           //
// This function is a fourier expansion                      //
//                                                           //
// ----------------------------------------------------------// 
#ifndef _FOURIEREXPANSION_H_
#define _FOURIEREXPANSION_H_
#include "QatGenericFunctions/AbsFunction.h"
#include "FourierCoefficientSet.h"
#include <stdexcept>

namespace Genfun {
  class FourierExpansion:public AbsFunction {

    // Required macro:
    FUNCTION_OBJECT_DEF(FourierExpansion)

  public:

    // The contructor
    FourierExpansion(const FourierCoefficientSet &coefficients, double scale=1.0);

    // This form of the function call operator cannot be used:
    double operator() (double ) const;

    // Function call operator, requires two dimensions
    double operator() (const Argument & a) const {
      if (a.dimension()!=1) throw std::runtime_error("FourierExpansion:  dimension mismatch");
      return operator()(a[0]);
    }


  private:

    FourierCoefficientSet c;
    double                scale;

  };
}
#endif

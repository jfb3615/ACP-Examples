//-----------------------------------------------------------//
//                                                           //
// This function is a dual (x,y) fourier expansion           //
//                                                           //
// ----------------------------------------------------------// 
#ifndef _DUALFOURIEREXPANSION_H_
#define _DUALFOURIEREXPANSION_H_
#include "QatGenericFunctions/AbsFunction.h"
#include "DualFourierCoefficientSet.h"

namespace Genfun {

  class DualFourierExpansion:public AbsFunction {

    // Required macro:
    FUNCTION_OBJECT_DEF(DualFourierExpansion)

  public:

    // The contructor
    DualFourierExpansion(const DualFourierCoefficientSet &coefficients, 
			 double scalex=1.0,
			 double scaley=1.0);

    // This form of the function call operator cannot be used:
    double operator() (double ) const {
      throw std::runtime_error("DualFourierExpansion:  dimension mismatch");
    }

    // Function call operator, requires two dimensions
    double operator() (const Argument & a) const;

  private:

    DualFourierCoefficientSet c;
    double                scaleX;
    double                scaleY;

  };
}
#endif

#include "QatGenericFunctions/AbsFunction.h"
//------------------------------------------------------------------------------------------------------------//
//                                                                                                                         //
//                                                                                                                         //
//                                                                                                                         //
//-----------------------------------------------------------------------------------------------------------//
#ifndef _CircuitIntegrationFunction_h_
#define _CircuitIntegrationFunction_h_
namespace Genfun {
  class CircuitIntegrationFunction:public AbsFunction {

    class CircuitIntegrator;
    
    FUNCTION_OBJECT_DEF(CircuitIntegrationFunction)

      public:
    
    // Constructor:
    CircuitIntegrationFunction();

    // Destructor:
    virtual ~CircuitIntegrationFunction();

    // Copy Constructor:
    CircuitIntegrationFunction (const CircuitIntegrationFunction & right);

    // Retrieve function value (here for function of one variable)
    virtual double operator ()(double argument) const;
    virtual double operator ()(const Argument & a) const {return operator() (a[0]);}

    // Derivative.  (here we provide)
    Derivative partial (unsigned int) const;

    // Does this function have an analytic derivative?  (here we say yes)
    virtual bool hasAnalyticDerivative() const {return true;}

  private:
    
    const CircuitIntegrationFunction & operator=(const CircuitIntegrationFunction &right)=delete;

   
  };
}
#endif

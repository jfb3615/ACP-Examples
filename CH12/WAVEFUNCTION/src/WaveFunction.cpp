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
// Here you have the .cpp file                               //
//                                                           //
// ----------------------------------------------------------// 

#include "WaveFunction.h"
#include <complex>
typedef std::complex<double> Complex;
namespace Genfun {

  // This macro defines a number of methods required
  // by the base class but which we generate here
  // automatically in the macro so you do not need to
  // know about those.. See the header file where a
  // corresponding macro is invoked. 
  FUNCTION_OBJECT_IMP(WaveFunction)

  // Implementation of the constructor.  Here we solve for the wavefunction
  // by matching the function and its first derivatives at the each boundary
  // in the piecewise constant potential.  This is done in the constructor so
  // that it does not need to be repeated each time the function is evaluated.
  WaveFunction::WaveFunction(const std::vector<Step> & steps) :step(steps) {
    static const Complex I(0,1);
    Eigen::MatrixXcd A=Eigen::MatrixXcd::Zero(2*step.size(),
					      2*step.size());
    //
    // The following express the continuity of the wavefunction
    //
    {
      double x=step[0].x;
      Complex n0=Complex(1.0,0);
      Complex n1=step[0].n;
      
      // First Equation:
      A(0,0) = +exp(-I*x*n0);
      A(0,1) = -exp(+I*x*n1);
      A(0,2) = -exp(-I*x*n1);
      
      // Middle Equations
      for (unsigned int i=1;i<step.size()-1;i++) {
	x=step[i].x;
	n0=step[i-1].n;
	n1=step[i].n;
	A(i,2*i-1)   =  +exp(+I*x*n0);
	A(i,2*i-0) =   +exp(-I*x*n0);
	A(i,2*i+1) =   -exp(+I*x*n1);
	A(i,2*i+2) =   -exp(-I*x*n1);
      }
      
      // Last Equation 
      x=step.back().x;
      n0=step[step.size()-2].n;
      n1=step.back().n;
      unsigned int index=step.size()-1;
      A(index, 2*index-1) = +exp (+I*x*n0);
      A(index, 2*index-0) = +exp (-I*x*n0);
      A(index, 2*index+1) = -exp (I*x*n1);
    }
    
    //
    // The following express the continuity of the slope of the wavefunction
    //
    {
      unsigned int shift=step.size();
      double x=step[0].x;
      Complex n0=Complex(1.0,0);
      Complex n1=step[0].n;
      
      // First Equation:
      A(shift+0,0) = +exp(-I*x*n0)*(-I*n0);
      A(shift+0,1) = -exp(+I*x*n1)*(+I*n1);
      A(shift+0,2) = -exp(-I*x*n1)*(-I*n1);
      
      // Middle Equations
      for (unsigned int i=1;i<step.size()-1;i++) {
	x=step[i].x;
	n0=step[i-1].n;
	n1=step[i].n;
	A(shift+i,2*i-1) =   +exp(+I*x*n0)*(+I*n0);
	A(shift+i,2*i-0) =   +exp(-I*x*n0)*(-I*n0);
	A(shift+i,2*i+1) =   -exp(+I*x*n1)*(+I*n1);
	A(shift+i,2*i+2) =   -exp(-I*x*n1)*(-I*n1);
      }
      
      // Last Equation 
      x=step.back().x;
      n0=step[step.size()-2].n;
      n1=step.back().n;
      unsigned int index=step.size()-1;
      A(shift+index, 2*index-1) = +exp (+I*x*n0)*(+I*n0);
      A(shift+index, 2*index-0) = +exp (-I*x*n0)*(-I*n0);
      A(shift+index, 2*index+1) = -exp (+I*x*n1)*(+I*n1);
    }
    Eigen::VectorXcd X=Eigen::VectorXcd::Zero(2*step.size());
    X[0]=-exp(I*step[0].x);
    X[step.size()] = -I*exp(I*step[0].x);
    
    sVector=A.inverse()*X;
  }

  // Function call operator.  Here we evaluate the function given
  // the solution which was calculated in the constructor. 
  double WaveFunction::operator() (double x) const {

    static const Complex I(0,1);
    if (x<=step.front().x) {
      Complex psi=exp(I*x)+sVector[0]*exp(-I*x);
      return norm(psi);
    }
    else if (x>=step.back().x) {
      Complex psi=sVector[sVector.rows()-1]*exp(I*step.back().n*x);
      return norm(psi);
    }
    else {
      for (unsigned int i=1;i<step.size();i++) {
	if (x<step[i].x) {
	  Complex psi=sVector[2*i-1]*exp(I*step[i-1].n*x) + sVector[2*i]*exp(-I*step[i-1].n*x);
	  return norm(psi);
	}
      }
    }
    return 0;
  }
}

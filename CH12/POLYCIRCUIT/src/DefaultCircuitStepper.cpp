#include "DefaultCircuitStepper.h"
#include "CircuitIntegrator.h"
#include "Eigen/Dense"
#include "QatGenericFunctions/InterpolatingPolynomial.h"
#include "QatGenericFunctions/Sigma.h"
#include <deque>

namespace circuit {

  DefaultCircuitStepper::DefaultCircuitStepper(const Circuit & circuit):
    circuit(circuit),monger(circuit),
    N{monger.getNumPaths()},
    M{circuit.elements().size()},
    A{Eigen::MatrixXi::Zero(N,M)}
{

  //    const size_t N{monger.getNumPaths()};
  // const size_t M{circuit.elements().size()};					 
    // Associate an independent current with each branch. 
    
    // For each path i, for each element j=:
    // Fill with "1" if element is traversed forward.
    // Fill with "-1" if element is traversed in reverse;
    // Fill with "0"  if element is not traversed in path:

    for (int k=0;k<M;k++) {
      const auto & e=circuit.elements()[k];
      for (int j=0;j<N;j++ ) {
	const auto & p=monger.getPath(j);
	if (p.contains(e.get())) {
	  const Path::Terminal *t=p.get(e.get());
	  if (t) {
	    if (t->terminal==1) {
	      A(j,k)=1;
	    }
	    else {
	      A(j,k)=-1;
	    }
	  }
	}
      }
    }
    GF.resize(3*N);


    {
      for (size_t i=0;i<N;i++) {
	size_t c{0};
	const Path & p=monger.getPath(i);
	for (size_t k=0;k<M;k++) {
	  const auto & e=circuit.elements()[k];    // k. the element
	  if (p.contains(e.get())) { 
	    const Genfun::AbsFunction *vVsT=e->voltageVsTime();
	    if (vVsT) {
	      int sign=A(i,k);
	      GF[c].accumulate(*vVsT*sign);
	      GF[c+1].accumulate(vVsT->prime()*sign);
	      GF[c+2].accumulate(vVsT->prime().prime()*sign);
	    }
	  }
	}
	c+=3;
      }
    }
    VF=Eigen::Array<DirectedElement,Eigen::Dynamic,Eigen::Dynamic>(N,M);
    {
      for (size_t i=0;i<N;i++) {
	const Path & p=monger.getPath(i);
	for (size_t k=0;k<M;k++) {
	  const auto & e=circuit.elements()[k];
	  int sign=A(i,k);
	  if (p.contains(e.get())) {
	    VF(i,k)=std::make_pair(e.get(),sign);
	  }
	  else {
	    VF(i,k)=std::make_pair(nullptr,sign);
	  }
	}
      }
    }
  }

  
  DefaultCircuitStepper::~DefaultCircuitStepper() {
  }
  
  void DefaultCircuitStepper::step (const std::shared_ptr<CircuitIntegrator::CircuitData>  & data, 
				    const CircuitIntegrator::CircuitData::Data             & sdata, 
				    CircuitIntegrator::CircuitData::Data                   & ddata, 
				    double timeLimit) const {

    const double stepsize=0.01;
    const double h = timeLimit==0 ? stepsize : timeLimit - sdata.time;
    double time= sdata.time+h;
    
    // Compute the vector of impressed voltages (first derivatives, second derivatives)
    Eigen::VectorXd G=Eigen::VectorXd::Zero(3*N);
    Eigen::VectorXd X0=Eigen::VectorXd::Zero(3*N);
    for (int i=0;i<sdata.variable.size();i++) {
      G(i)=GF[i](time);
      X0(i)=sdata.variable[i];
    }

    
    Eigen::VectorXd F0=doStep(X0, G, time);
    Eigen::MatrixXd Jacobian=Eigen::MatrixXd::Zero(3*N,3*N);
    for (int i=0;i<X0.size();i++) {
      {
	Eigen::VectorXd XPlus{X0},XMinus{X0};
	XPlus(i) +=1.0E-8;	
	XMinus(i)-=1.0E-8;
	Eigen::Vector XUp=doStep(XPlus,  G, time);
	Eigen::Vector XDn=doStep(XMinus, G, time);
	Jacobian.col(i)= (XUp-XDn)/2.0E-8;
      }
    }
    X0-=Jacobian.inverse()*F0;

    for (int i=0;i<3*N;i++) {
      ddata.variable[i]=X0(i);
    }
    ddata.time=time;

  }

  Eigen::VectorXd DefaultCircuitStepper::doStep(const Eigen::VectorXd & X0,
						const Eigen::VectorXd & G,
						double time,
						const std::optional<int>  &) const {
    using namespace Genfun;
    
    Variable T;
    GENFUNCTION T1=T-time;
    GENFUNCTION T2=T1*T1;

    // For each element, construct the sum of currents:
    std::vector<Genfun::Sigma> sumCurrents(M);
    for (int k=0;k<M;k++) {
      size_t c{0};
      for (int j=0;j<N;j++) {
	int sign = A(j,k);
	double C0=sign*X0[c];
	double C1=sign*X0[c+1];
	double C2=sign*X0[c+2];
	GENFUNCTION IFromElement=C0+C1*T1+C2*T2;
        sumCurrents[k].accumulate(IFromElement);
	c+=3;
      }
    }
    
    Eigen::VectorXd F=Eigen::VectorXd::Zero(3*N);
    size_t c{0};
    for (int i=0;i<N;i++) {
      double VSum{0},VSumPrime{0},VSumPrimePrime{0};
      for (int k=0;k<M;k++) {
	if (A(i,k)!=0) {
	  auto [element, sign] =VF(i,k);
	  if (element) {
	    auto vFromI=element->voltageFromCurrent(sumCurrents[k]);
	    if (vFromI) {
	      VSum           +=sign*((*vFromI)(time));
	      VSumPrime      +=sign*((vFromI->prime())(time));
	      VSumPrimePrime +=sign*((vFromI->prime().prime())(time));
	    }
	  }
	}
      }
      F(c)=VSum;
      F(c+1)=VSumPrime;
      F(c+2)=VSumPrimePrime;
      c+=3;
    }
    return F+G;
  }
}
  
// Path 0 Current 0 Deriv 0
// Path 0 Current 0 Deriv 1
// Path 0 Current 0 Deriv 2
// Path 0 Current 1 Deriv 0
// Path 0 Current 1 Deriv 1
// Path 0 Current 1 Deriv 2
// Path 1 Current 0 Deriv 0
// Path 1 Current 0 Deriv 1
// Path 1 Current 0 Deriv 2
// Path 1 Current 1 Deriv 0
// Path 1 Current 1 Deriv 1
// Path 1 Current 1 Deriv 2
///...

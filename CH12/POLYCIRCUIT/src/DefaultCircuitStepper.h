#pragma once
#include "CircuitIntegrator.h"
#include "QatGenericFunctions/Variable.h"
#include "QatGenericFunctions/Sigma.h"
#include "Circuit/Path.h"
#include "Eigen/Dense"
#include <deque>
#include <optional>
namespace circuit {
  class Circuit;
  class PathMonger;
  
  class DefaultCircuitStepper: public CircuitIntegrator::CircuitStepper{

  public:
    
    DefaultCircuitStepper(const Circuit & circuit);
    
    virtual ~DefaultCircuitStepper();
    virtual void step (const std::shared_ptr<CircuitIntegrator::CircuitData>  & data, 
		       const CircuitIntegrator::CircuitData::Data             & sdata, 
		       CircuitIntegrator::CircuitData::Data                   & ddata, 
		       double timeLimit=0) const override final;
  private:

    Eigen::VectorXd doStep(const Eigen::VectorXd & inputVariables,
			   const Eigen::VectorXd & impressedVAndPrimes,
			   double time,
			   const std::optional<int> &index=std::optional<int>() ) const;
    
    using SystemHistory = std::deque< CircuitIntegrator::CircuitData::Data>;
    const Circuit & circuit;
    const PathMonger monger;
    const size_t N;
    const size_t M;
    Eigen::MatrixXi A;
    mutable SystemHistory      history;

    const double deltaT{0.001};
    Genfun::Variable T;
    Genfun::GENFUNCTION F0{T*(T-deltaT)/(2.0*deltaT*deltaT)};
    Genfun::GENFUNCTION F1{-(T+deltaT)*(T-deltaT)/(deltaT*deltaT)};
    Genfun::GENFUNCTION F2{T*(T+deltaT)/(2.0*deltaT*deltaT)};
    mutable std::deque<Eigen::VectorXd> vHistory;    

    using DirectedElement=std::pair<const CircuitElement *,int>;
    std::vector<Genfun::Sigma>   GF; // 
    Eigen::Array<DirectedElement,Eigen::Dynamic,Eigen::Dynamic> VF; // N*M
  };
}


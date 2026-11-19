#include "Circuit/CircuitSimulator.h"
#include "Circuit/Path.h"
#include "QatGenericFunctions/FixedConstant.h"
#include "QatGenericFunctions/Sigma.h"
#include "Eigen/Dense"
#include "CircuitIntegrator.h"

#include <map>

namespace circuit {

  // Nested implementation map.  This holds the private member data
  // for the CircuitSimulator. 
  class CircuitSimulator::Clockwork {
    
  public:

    std::map<const CircuitElement *, std::unique_ptr<Genfun::AbsFunction>>  currentMap;
    std::map<const CircuitNode    *, std::unique_ptr<Genfun::AbsFunction>>  voltageMap;
    std::unique_ptr<CircuitIntegrator>                                      integrator;
  };

  //====================================================================
  //====================================================================

  // Constructor
  CircuitSimulator::CircuitSimulator(const Circuit& circuit):
    cw_{std::make_unique<Clockwork> ()}
  {
    for (const auto & e : circuit.elements()) {
      cw_->currentMap[e.get()]=std::make_unique<Genfun::FixedConstant>(0.75);
    }
    for (const auto & n : circuit.nodes()) {
      cw_->voltageMap[n.get()]=std::make_unique<Genfun::FixedConstant>(0.75);
    }

   
    //unsigned int N=monger.getNumPaths();
    cw_->integrator=std::make_unique<CircuitIntegrator>(circuit);
  }

  // Destructor;
  CircuitSimulator::~CircuitSimulator()=default;
  
  // For a given element in the circuit, obtain the current into the element:
  const Genfun::AbsFunction * CircuitSimulator::currentInto( const CircuitElement *element, size_t terminal) const {
    auto f=cw_->currentMap.find(element);
    if (f!=cw_->currentMap.end()) {
      return f->second.get();
    }
    else {
      throw std::runtime_error("ERROR: Element "+element->name()+" does not belong to the circuit");
      return nullptr;
    }
  }

  // For a given element in the circuit, obtain the voltage between terminals (0 and 1 by default).
  const Genfun::AbsFunction * CircuitSimulator::voltageAt ( const CircuitNode *node) const{
    auto f=cw_->voltageMap.find(node);
    if (f!=cw_->voltageMap.end()) {
      return f->second.get();
    }
    else {
      throw std::runtime_error("ERROR: Node is not found in the circuit");
    }
  }
  const size_t CircuitSimulator::getNumCurrents() const {
    return cw_->integrator->getNumIndependentCurrents();
  }
  
  const Genfun::AbsFunction *CircuitSimulator::getCurrent(size_t i) const {
    return cw_->integrator->getIndependentCurrent(i);
  }
  
  
} // namespace circuit

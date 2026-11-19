#pragma once

#include "Circuit/Circuit.h"
#include "QatGenericFunctions/AbsFunction.h"
#include <memory>
#include <string>
#include <vector>


namespace circuit {

class CircuitSimulator {
public:

  // Constructor I
  CircuitSimulator(const Circuit& circuit);

  // Destructor;
  ~CircuitSimulator();
  
  // Hide copy constructor:
  CircuitSimulator(const CircuitSimulator&) = delete;

  // Hide assignment operator:
  CircuitSimulator& operator=(const CircuitSimulator&) = delete;

  // Default Move operator:
  CircuitSimulator(CircuitSimulator&&) = default;

  // Default Move Assignment:
  CircuitSimulator& operator=(CircuitSimulator&&)=default;

  // For a given element in the circuit, obtain the current into the element:
  const Genfun::AbsFunction * currentInto( const CircuitElement *, size_t terminal=0) const;

  // For a given element in the circuit, obtain the voltage between terminals (0 and 1 by default).
  const Genfun::AbsFunction * voltageAt ( const CircuitNode * node) const;

  const size_t getNumCurrents() const;
  const Genfun::AbsFunction *getCurrent(size_t i) const;
  

private:
    class Clockwork;
    std::unique_ptr<Clockwork> cw_;
};

} // namespace circuit

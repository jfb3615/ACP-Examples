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
  CircuitSimulator(const Circuit& circuit, double initialCapacitorVoltage = 0.0, double initialInductorCurrent = 0.0);

  // Constructor II
  CircuitSimulator(const Circuit& circuit,std::vector<double> initialCapacitorVoltages, std::vector<double> initialInductorCurrents);

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

  // Number of independent variables:
  std::size_t numStateVariables() const ;

  // Names of independent variables:
  const std::vector<std::string>& independentVariables() const ;

  // Get the State function
  Genfun::GENFUNCTION stateFunction(std::size_t index) const;

private:
    class Clockwork;
    std::unique_ptr<Clockwork> cw_;
};

} // namespace circuit

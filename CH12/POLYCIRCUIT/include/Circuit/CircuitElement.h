#pragma once

#include "Circuit/CircuitNode.h"

#include <QatGenericFunctions/AbsFunction.h>

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

namespace circuit {

class CircuitElement {
public:

  //  Constructor: 
  CircuitElement(std::string name, std::size_t terminalCount);

  //  Virtual Destructor
  virtual ~CircuitElement() = default;

  // Name:
  const std::string& name() const ;

  // Number Of terminals
  std::size_t terminalCount() const ;

  // Read/write access to the terminal:
  CircuitNode* terminal(std::size_t index);

  // Readonly access to the terminal for const objects
  const CircuitNode* terminal(std::size_t index) const;

  // Make a connection:
  void connectTerminal(std::size_t index, CircuitNode& node);

  // Brack a connection:
  void disconnectTerminal(std::size_t index);

  // Gives the current to voltage characteristics
  virtual std::unique_ptr<Genfun::AbsFunction> currentFromVoltage(Genfun::GENFUNCTION V, double V0 = 0.0) const = 0;

  // Gives the voltage to current characteristics
  virtual std::unique_ptr<Genfun::AbsFunction> voltageFromCurrent(Genfun::GENFUNCTION I, double I0 = 0.0) const = 0;

  // Type identification
  virtual std::string type() const  = 0;

  // Symbol
  virtual const std::vector<std::pair<double,double>> & symbol() const;
  
private:

  CircuitElement(const CircuitElement&) = delete;
  CircuitElement& operator=(const CircuitElement&) = delete;
  
  std::string name_;
  std::vector<CircuitNode*> terminals_;
};

} // namespace circuit

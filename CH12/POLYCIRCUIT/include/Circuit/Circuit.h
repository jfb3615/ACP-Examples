#pragma once

#include "Circuit/CircuitElement.h"
#include "Circuit/CircuitNode.h"

#include <memory>
#include <vector>

namespace circuit {
  
  class Circuit {
  public:
    Circuit() = default;
    
    // Copy and assign are turned off.  Move operations allowed:
    Circuit(const Circuit&) = delete;
    Circuit& operator=(const Circuit&) = delete;
    Circuit(Circuit&&)  = default;
    Circuit& operator=(Circuit&&)  = default;
    
    // Add connection node to the circuit
    CircuitNode& addNode(Point position = {});

    // Add an element to the circuit. Circuit owns element. 
    CircuitElement& addElement(std::unique_ptr<CircuitElement> element);

    // Access to nodes:
    const std::vector<std::unique_ptr<CircuitNode>>& nodes() const ;

    // Access to nodes:
    const std::vector<std::unique_ptr<CircuitElement>>& elements() const ;
    
    // Reduce wires (absorb them into node for simulation).
    void reduce();
    
  private:
    std::vector<std::unique_ptr<CircuitNode>> nodes_;
    std::vector<std::unique_ptr<CircuitElement>> elements_;
    CircuitNode::Id nextNodeId_{};
  };
  
  // Utilities:
  size_t countLoops(const circuit::Circuit & model);
  
} // namespace circuit


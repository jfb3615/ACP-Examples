#include "Circuit/CircuitElement.h"

#include <stdexcept>
#include <utility>

namespace circuit {
  
  CircuitElement::CircuitElement(std::string name, std::size_t terminalCount)
    : name_(std::move(name)), terminals_(terminalCount, nullptr) {}
  
  const std::string& CircuitElement::name() const  {
    return name_;
  }
  
  std::size_t CircuitElement::terminalCount() const  {
    return terminals_.size();
  }
  
  CircuitNode* CircuitElement::terminal(std::size_t index) {
    if (index >= terminals_.size()) {
      throw std::out_of_range("Circuit element terminal index is out of range");
    }
    return terminals_[index];
  }
  
  const CircuitNode* CircuitElement::terminal(std::size_t index) const {
    if (index >= terminals_.size()) {
      throw std::out_of_range("Circuit element terminal index is out of range");
    }
    return terminals_[index];
  }
  
  void CircuitElement::connectTerminal(std::size_t index, CircuitNode& node) {
    if (index >= terminals_.size()) {
      throw std::out_of_range("Circuit element terminal index is out of range");
    }
    terminals_[index] = &node;
  }
  
  void CircuitElement::disconnectTerminal(std::size_t index) {
    if (index >= terminals_.size()) {
      throw std::out_of_range("Circuit element terminal index is out of range");
    }
    terminals_[index] = nullptr;
  }
  
  const std::vector<std::pair<double,double>> &  CircuitElement::symbol() const {
    static std::vector<std::pair<double,double>> mySymbol{{-25.0,-10.0},
							  {-25.0,+10.0},
							  {+25.0,+10.0},
							  {+25.0,-10.0},
							  {-25.0, -10.0}};
    return mySymbol;
  }

  
} // namespace circuit

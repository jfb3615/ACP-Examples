#pragma once

#include "Circuit/CircuitElement.h"
#include "Circuit/CircuitNode.h"

#include <memory>
#include <vector>

namespace circuit {

class Circuit {
public:
    Circuit() = default;

    Circuit(const Circuit&) = delete;
    Circuit& operator=(const Circuit&) = delete;
    Circuit(Circuit&&)  = default;
    Circuit& operator=(Circuit&&)  = default;

    CircuitNode& addNode(Point position = {});
    CircuitElement& addElement(std::unique_ptr<CircuitElement> element);

    const std::vector<std::unique_ptr<CircuitNode>>& nodes() const ;
    const std::vector<std::unique_ptr<CircuitElement>>& elements() const ;

    void reduce();


  
private:
    std::vector<std::unique_ptr<CircuitNode>> nodes_;
    std::vector<std::unique_ptr<CircuitElement>> elements_;
    CircuitNode::Id nextNodeId_{};
};

} // namespace circuit

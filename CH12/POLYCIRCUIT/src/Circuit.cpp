#include "Circuit/Circuit.h"
#include "QatGenericFunctions/FixedConstant.h"
#include <limits>
#include <stdexcept>
#include <utility>
#include <algorithm>

namespace circuit {

CircuitNode& Circuit::addNode(Point position) {
    if (nextNodeId_ == std::numeric_limits<CircuitNode::Id>::max()) {
        throw std::overflow_error("Circuit node ID space is exhausted");
    }

    auto node = std::make_unique<CircuitNode>(nextNodeId_++, position);
    CircuitNode& result = *node;
    nodes_.push_back(std::move(node));
    return result;
}

CircuitElement& Circuit::addElement(std::unique_ptr<CircuitElement> element) {
    if (!element) {
        throw std::invalid_argument("Circuit element must not be null");
    }

    CircuitElement& result = *element;
    elements_.push_back(std::move(element));
    return result;
}

const std::vector<std::unique_ptr<CircuitNode>>& Circuit::nodes() const  {
    return nodes_;
}

const std::vector<std::unique_ptr<CircuitElement>>& Circuit::elements() const  {
    return elements_;
}

void Circuit::reduce() {

  bool repeatNeeded{true};
  while (repeatNeeded) {
    repeatNeeded=false;
    CircuitNode *circuitNode0{nullptr};
    CircuitNode *circuitNode1{nullptr};;
    
    for (auto i=elements_.begin();i!=elements_.end();i++) {
      auto & e=*i;
      if (e->type()=="Wire")  {
	circuitNode0=e->terminal(0);
	circuitNode1=e->terminal(1);
	
	e->disconnectTerminal(0);
	e->disconnectTerminal(1);
	elements_.erase(i);
	repeatNeeded=true;
	break;
      }
    }
    if (circuitNode0 && circuitNode1) {
      for (auto i=elements_.begin();i!=elements_.end();i++) {
	auto & e=*i;
	if (e->terminal(0)==circuitNode0) {
	  e->connectTerminal(0,*circuitNode1);
	}
	else if (e->terminal(1)==circuitNode0) {
	  e->connectTerminal(1,*circuitNode1);
	}
      }
    }
    auto nodeRemoval=std::find_if(nodes_.begin(),nodes_.end(), [circuitNode0] (std::unique_ptr<CircuitNode> & n) {return n.get()==circuitNode0;});
    if (nodeRemoval!=nodes_.end()) nodes_.erase(nodeRemoval);
  } 
}

  
} // namespace circuit

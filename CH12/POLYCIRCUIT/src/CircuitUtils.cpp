#include "Circuit/Circuit.h"
#include <unordered_map> 
std::size_t root(std::vector<std::size_t>& parents, std::size_t index) {
    while (parents[index] != index) {
        parents[index] = parents[parents[index]];
        index = parents[index];
    }
    return index;
}

namespace circuit {
  size_t countLoops(const circuit::Circuit & model) {

    // agglomerate connected nodes connected to the first node presented. 
    auto rootNode = [](std::vector<std::size_t>& parents, std::size_t index) {
      while (parents[index] != index) {
        parents[index] = parents[parents[index]];
        index = parents[index];
      }
      return index;
    };
    
    std::unordered_map<const circuit::CircuitNode*, std::size_t> indices;
    indices.reserve(model.nodes().size());
    for (std::size_t i = 0; i < model.nodes().size(); ++i) {
      indices.emplace(model.nodes()[i].get(), i);
    }
    std::vector<std::size_t> parents(model.nodes().size());
    for (std::size_t i = 0; i < parents.size(); ++i) {
      parents[i] = i;
    }
    std::size_t independentLoops = 0;
    for (const auto& element : model.elements()) {
      if (element->terminalCount() != 2 || !element->terminal(0) ||
	  !element->terminal(1)) {
	std::cerr << "each component must connect to exactly two nodes" << std::endl;
	return 0;
      }
      std::size_t first = rootNode(parents, indices.at(element->terminal(0)));
      std::size_t second = rootNode(parents, indices.at(element->terminal(1)));
      if (first == second) {
	++independentLoops;
      } else {
	parents[first] = second;
      }
    }
    return independentLoops;
  }
}

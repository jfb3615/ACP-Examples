#include "Circuit/Circuit.h"
#include "Circuit/Path.h"
#include <unordered_map>
#include <limits>

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
    for (const auto & element: model.elements()) {
      int rn=rootNode(parents,indices.at(element->terminal(0)));
    }
    return independentLoops;
  }


  // Recursive routine:
  // Look at the end of the path:
  // For each direction:
  // Check if the path connects back to the original, if so add that option to the path and return.
  // Otherwise check all of the other paths for their distance.
  // Choose the shortest.
  
  
  size_t getOptimalPath(Path & path, const Circuit & circuit) {
    Path::Terminal homeTerminal=path.head();
    Path::Terminal lastTerminal=path.tail();
    std::vector<Path> exploratoryPaths;
    for (auto & e : circuit.elements()) {
      
      // Do not consider any element already present:
      if (path.contains(e.get())) continue;
      
      if (e->terminal(0)==lastTerminal.element->terminal(lastTerminal.terminal)) {
	Path newPath=path;
	newPath.add(e.get(),1);
	exploratoryPaths.push_back(newPath);
      }
      if (e->terminal(1)==lastTerminal.element->terminal(lastTerminal.terminal)) {
	Path newPath=path;
	newPath.add(e.get(),0);
	exploratoryPaths.push_back(newPath);
      }
    }
    size_t shortestSize=std::numeric_limits<size_t>::max();
    if (exploratoryPaths.empty()) return shortestSize; 
    for (Path & p : exploratoryPaths) {
      size_t otherEnd=(p.tail().terminal)%2;
      if (p.tail().element->terminal(otherEnd)==homeTerminal.element->terminal(0)) { // Path has closed
	path=p;
	return path.distance();
      }
      else {
	size_t size=getOptimalPath(p,circuit);
	if (size<shortestSize) {
	  shortestSize=size;
	  path=p;
	}
      }
    }
    return shortestSize;
  }
  
  Path getOptimalPath(const Circuit & circuit, const CircuitElement*element) {
    Path p;
    p.add(element,1);
    getOptimalPath(p,circuit);
    return p;  
  }

  PathMonger::PathMonger(const Circuit & circuit) {
    // Empty?  There are no paths:
    if (circuit.elements().empty()) return;

    // Loop over all circuit elements.  Choose the first element that
    // has not previously been included in a loop. Find the optimal loop
    // for that element. Repeat until all circuit elements are included
    // in at least one loop.
    
    bool working{true};
    while (working)  {
      working=false;
      for (auto & e : circuit.elements()) {
	const CircuitElement *selectedElement{nullptr};
	bool exclude{false};
	for (const Path & p: _pathVector) {
	  if (p.contains(e.get())) {
	    exclude=true;
	    break;
	  }
	}
	if (!exclude) {
	  selectedElement=e.get();
	  _pathVector.emplace_back(getOptimalPath(circuit,selectedElement));
	  working=true;
	  break;
	}
      }
    }
  }
}

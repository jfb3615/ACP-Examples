// This class represents a closed loop in a circuit.  It may be used
// in the application of Kirchoff's laws. This presupposes only bipolar
// devices.  Those needing more than that should get a professional
// package. 
#pragma once
#include "Circuit/Circuit.h"
#include <algorithm>
namespace circuit {


  class Path {

  public:

    // Nested class records not only which circuit element but which terminal
    // is connected to the previous element in the path. 
    struct Terminal {
      const CircuitElement *element;
      size_t                terminal;
    };

    // Add an element to the path. 
    void add(const CircuitElement *element,size_t terminal) {
      _elementList.push_back({element,terminal});
    }

    // How long is the path? 
    size_t distance() const {
      return _elementList.size();
    }

    // Determine whether the path contains an element:
    bool contains(const CircuitElement *element) const {
      return std::find_if(_elementList.begin(),_elementList.end(), [ & element] (const Terminal  &t) {return t.element==element;}) !=_elementList.end();
    }

    // Get the tail:
    const Terminal & tail () const {
      if (_elementList.empty()) throw std::runtime_error("Error, attempted access on empty path");
      return _elementList.back();
    }

    // Get the head? 
    const Terminal & head () const {
      if (_elementList.empty()) throw std::runtime_error("Error, attempted access on empty path");
      return _elementList.front();
    }

    // Random access:
    const Terminal & operator[] (int i) const { return _elementList[i];}

    // Get by element:
    const Terminal * get(const CircuitElement *element) const {
      auto t=std::find_if(_elementList.begin(),_elementList.end(), [& element] (const Terminal & T) {return T.element==element;});
      if (t!=_elementList.end()) return &*t;
      return nullptr;
    }
    
    // Iteration:
    using ConstIterator=std::vector<Terminal>::const_iterator;
    ConstIterator begin() const {return _elementList.begin();}
    ConstIterator end()   const {return _elementList.end();}

    
  private:
    
    std::vector<Terminal> _elementList;
    
  };


  class PathMonger{

  public:
    
    // Get a list of independent closed loops:
    PathMonger(const Circuit & circuit);

    // Retrieve the number of independent paths:
    size_t getNumPaths() const {return _pathVector.size();}

    // Retreive the i^th path:
    const Path & getPath(int i) const {return _pathVector[i];}

    // Iteration:
    using ConstIterator=std::vector<Path>::const_iterator;
    ConstIterator begin() const { return _pathVector.begin();}
    ConstIterator end() const   {return _pathVector.end();}

  private:

    std::vector<Path> _pathVector;

    
  };

  // Routine to obtain a shortest path through the circuit containing
  // a specified element.   
  Path getOptimalPath(const Circuit & circuit, const CircuitElement*element);

}

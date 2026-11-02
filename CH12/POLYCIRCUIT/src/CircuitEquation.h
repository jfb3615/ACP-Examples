#pragma once
#include "QatGenericFunctions/AbsFunction.h"
#include "CircuitSystem.h"


namespace circuit {
  namespace {
    // Move me
    std::vector<double> evaluateDerivatives(const CircuitSystem& system, const Genfun::Argument& state);
    //
  }

  
  class CircuitEquation final : public Genfun::AbsFunction {
  public:
    using Genfun::AbsFunction::operator();
    
  CircuitEquation(std::shared_ptr<const CircuitSystem> system, std::size_t index)
    : system_(std::move(system)), index_(index) {}
    
    unsigned int dimensionality() const override {
      return static_cast<unsigned int>(system_->stateNames.size() + 1);
    }
    
    double operator()(double time) const override {
      if (dimensionality()!=1) throw std::runtime_error("CircuitEquation: dimensionality error in operator ()"); 
      Genfun::Argument state(1, time);
      return (*this)(state);
    }
    
    double operator()(const Genfun::Argument& state) const override {
       return evaluateDerivatives(*system_, state).at(index_);
    }
    
    CircuitEquation* clone() const override {
      return new CircuitEquation(*this);
    }
  private:
    std::shared_ptr<const CircuitSystem> system_;
    std::size_t index_;
  };
}

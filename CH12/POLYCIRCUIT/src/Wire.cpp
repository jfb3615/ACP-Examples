#include "Circuit/Wire.h"
#include "ConstitutiveFunction.h"

#include <cmath>
#include <stdexcept>
#include <utility>

namespace circuit {

Wire::Wire(std::string name)
    : CircuitElement(std::move(name), 2) {
}

std::string Wire::type() const  {
    return "Wire";
}



std::unique_ptr<Genfun::AbsFunction> Wire::currentFromVoltage(Genfun::GENFUNCTION voltage, double) const {
  throw std::runtime_error("Wire current infinite for nonzero voltage");
}

std::unique_ptr<Genfun::AbsFunction> Wire::voltageFromCurrent(Genfun::GENFUNCTION current, double) const {
  return detail::transformFunction(current, detail::Operation::Scale,0.0);
}
  const std::vector<std::pair<double,double>> &  Wire::symbol() const {
    static std::vector<std::pair<double,double>> mySymbol{{-25.0, 0.0},
							  {25.0, 0.0}};
    return mySymbol;
  }
  
  
} // namespace circuit

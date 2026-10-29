#include "Circuit/Resistor.h"
#include "ConstitutiveFunction.h"

#include <cmath>
#include <stdexcept>
#include <utility>

namespace circuit {
  
  Resistor::Resistor(std::string name, double resistanceOhms)
    : CircuitElement(std::move(name), 2), resistanceOhms_(resistanceOhms) {
    if (!std::isfinite(resistanceOhms_) || resistanceOhms_ <= 0.0) {
      throw std::invalid_argument("Resistance must be finite and positive");
    }
  }
  
  std::string Resistor::type() const  {
    return "Resistor";
  }
  
  double Resistor::resistanceOhms() const  {
    return resistanceOhms_;
  }
  
  double Resistor::currentForVoltageDrop(double voltageDrop) const  {
    return voltageDrop / resistanceOhms_;
  }
  
  double Resistor::currentForVoltageDrop(double voltageDrop, double resistanceOhms) {
    if (!std::isfinite(resistanceOhms) || resistanceOhms <= 0.0) {
      throw std::invalid_argument("Resistance must be finite and positive");
    }
    return voltageDrop / resistanceOhms;
  }
  
  std::unique_ptr<Genfun::AbsFunction> Resistor::currentFromVoltage(Genfun::GENFUNCTION voltage, double) const {
    return detail::transformFunction(voltage, detail::Operation::Scale,1.0 / resistanceOhms_);
  }
  
  std::unique_ptr<Genfun::AbsFunction> Resistor::voltageFromCurrent(Genfun::GENFUNCTION current, double) const {
    return detail::transformFunction(current, detail::Operation::Scale,resistanceOhms_);
  }
  
  const std::vector<std::pair<double,double>> &  Resistor::symbol() const {
    static std::vector<std::pair<double,double>> mySymbol{{-25.0, 0.0},
							  {-17.0, 0.0},
							  {-12.0, -9.0},
							  {-6.0, 9.0},
							  {0.0, -9.0},
							  {6.0, 9.0},
							  {12.0, -9.0},
							  {17.0, 0.0},
							  {25.0, 0.0}};
    return mySymbol;
  }
  
  
} // namespace circuit

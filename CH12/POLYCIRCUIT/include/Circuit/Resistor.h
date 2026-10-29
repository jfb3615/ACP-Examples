#pragma once

#include "Circuit/CircuitElement.h"

#include <string>

namespace circuit {

class Resistor final : public CircuitElement {
public:

  // Constructor
  Resistor(std::string name, double resistanceOhms);

  // Type
  std::string type() const  override;

  // Get the resistance:
  double resistanceOhms() const ;

  // Current - voltage relations (Ohm's law):
  std::unique_ptr<Genfun::AbsFunction> currentFromVoltage(Genfun::GENFUNCTION V, double I0 = 0.0) const override;
  std::unique_ptr<Genfun::AbsFunction> voltageFromCurrent(Genfun::GENFUNCTION I, double V0 = 0.0) const override;

 // Symbol
  virtual const std::vector<std::pair<double,double>> & symbol() const override;
  
  
private:

  double resistanceOhms_;
  double currentForVoltageDrop(double voltageDrop) const ;
  static double currentForVoltageDrop(double voltageDrop, double resistanceOhms);

};

} // namespace circuit

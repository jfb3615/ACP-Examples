#pragma once

#include "Circuit/CircuitElement.h"

#include <string>

namespace circuit {

class Wire final : public CircuitElement {
public:

  Wire(std::string name);
  
  std::string type() const  override;
   
  std::unique_ptr<Genfun::AbsFunction> currentFromVoltage(Genfun::GENFUNCTION V, double I0 = 0.0) const override;
  std::unique_ptr<Genfun::AbsFunction> voltageFromCurrent(Genfun::GENFUNCTION I, double V0 = 0.0) const override;

  // Symbol
  virtual const std::vector<std::pair<double,double>> &  symbol() const;

private:

  double currentForVoltageDrop(double voltageDrop) const ;
  static double currentForVoltageDrop(double voltageDrop, double resistanceOhms);

};

} // namespace circuit

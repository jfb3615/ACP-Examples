#pragma once

#include "Circuit/CircuitElement.h"

#include <string>

namespace circuit {

class Inductor final : public CircuitElement {
public:

  // Constructor
  Inductor(std::string name, double inductanceHenrys);

  // Type
  std::string type() const  override;

  // Get the inductance:
  double inductanceHenrys() const ;

  // Current-voltage characteristics
  std::unique_ptr<Genfun::AbsFunction> currentFromVoltage(Genfun::GENFUNCTION V, double I0 = 0.0) const override;
  std::unique_ptr<Genfun::AbsFunction> voltageFromCurrent(Genfun::GENFUNCTION I, double V0 = 0.0) const override;

  // Symbol
  virtual const std::vector<std::pair<double,double>> &  symbol() const;

private:
  
    double inductanceHenrys_;
    double currentDerivativeForVoltageDrop(double voltageDrop) const ;
    static double currentDerivativeForVoltageDrop(double voltageDrop, double inductanceHenrys);
};

} // namespace circuit

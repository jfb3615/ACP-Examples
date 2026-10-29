#pragma once

#include "Circuit/CircuitElement.h"

#include <string>

namespace circuit {

class Capacitor final : public CircuitElement {
public:

  //Constructor;
  Capacitor(std::string name, double capacitanceFarads);

  // Type:
  std::string type() const  override;

  // Get the capacitance:
  double capacitanceFarads() const ;

  // Current-voltage characteristics 
  std::unique_ptr<Genfun::AbsFunction> currentFromVoltage(Genfun::GENFUNCTION V, double I0 = 0.0) const override;
  std::unique_ptr<Genfun::AbsFunction> voltageFromCurrent(Genfun::GENFUNCTION I, double V0 = 0.0) const override;

  // Symbol
  virtual const std::vector<std::pair<double,double>> &  symbol() const;
private:

    double capacitanceFarads_;
    double voltageDerivativeForCurrent(double currentAmps) const ;
    static double voltageDerivativeForCurrent(double currentAmps, double capacitanceFarads);

};

} // namespace circuit

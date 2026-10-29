#pragma once

#include "Circuit/CircuitElement.h"

#include <string>

namespace circuit {

class ACVoltageSource final : public CircuitElement {
public:

  //Constructor:
  ACVoltageSource(std::string name, double peakVoltageVolts,double frequencyHertz, double phaseRadians = 0.0);

  // Type:
  std::string type() const  override;

  // Get amplitude:
  double peakVoltageVolts() const ;

  // Get Frequency:
  double frequencyHertz() const ;

  // Get phase:
  double phaseRadians() const ;

  // current from voltage returns a null pointer:
  std::unique_ptr<Genfun::AbsFunction> currentFromVoltage(Genfun::GENFUNCTION voltage, double initialCurrent = 0.0) const override;

  // voltage for an ideal voltage source is independent of current:
  std::unique_ptr<Genfun::AbsFunction> voltageFromCurrent(Genfun::GENFUNCTION current, double initialVoltage = 0.0) const override;

  // Symbol
  virtual const std::vector<std::pair<double,double>> &  symbol() const override;
  
private:

  double peakVoltageVolts_;
  double frequencyHertz_;
  double phaseRadians_;
  
  double voltageAt(double timeSeconds) const ;

};

} // namespace circuit

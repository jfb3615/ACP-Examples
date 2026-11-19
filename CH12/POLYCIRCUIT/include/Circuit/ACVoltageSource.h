#pragma once

#include "Circuit/CircuitElement.h"

#include <string>

namespace circuit {

class ACVoltageSource final : public CircuitElement {
public:

  //Constructor:
  ACVoltageSource(const std::string & name, double peakVoltageVolts,double frequencyHertz, double phaseRadians = 0.0);

  // Type:
  std::string type() const  override;

  // Get amplitude:
  double peakVoltageVolts() const ;

  // Get Frequency:
  double frequencyHertz() const ;

  // Get phase:
  double phaseRadians() const ;

  // voltage for an ideal voltage source is independent of current:
  virtual const Genfun::AbsFunction *voltageVsTime() const override;

  // Symbol
  virtual const std::vector<std::pair<double,double>> &  symbol() const override;
  virtual const std::vector<std::pair<SymbolSegmentType, size_t>> & symbolTypeAndNumber() const override;

  
private:

  double peakVoltageVolts_;
  double frequencyHertz_;
  double phaseRadians_;
  std::unique_ptr<const Genfun::AbsFunction>  function_;
  double voltageAt(double timeSeconds) const ;

};

} // namespace circuit

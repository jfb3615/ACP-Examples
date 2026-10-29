#include "Circuit/Capacitor.h"
#include "ConstitutiveFunction.h"

#include <cmath>
#include <stdexcept>
#include <utility>

namespace circuit {

Capacitor::Capacitor(std::string name, double capacitanceFarads)
    : CircuitElement(std::move(name), 2), capacitanceFarads_(capacitanceFarads) {
    if (!std::isfinite(capacitanceFarads_) || capacitanceFarads_ <= 0.0) {
        throw std::invalid_argument("Capacitance must be finite and positive");
    }
}

std::string Capacitor::type() const  {
    return "Capacitor";
}

double Capacitor::capacitanceFarads() const  {
    return capacitanceFarads_;
}

double Capacitor::voltageDerivativeForCurrent(double currentAmps) const  {
    return currentAmps / capacitanceFarads_;
}

double Capacitor::voltageDerivativeForCurrent(double currentAmps,
                                              double capacitanceFarads) {
    if (!std::isfinite(capacitanceFarads) || capacitanceFarads <= 0.0) {
        throw std::invalid_argument("Capacitance must be finite and positive");
    }
    return currentAmps / capacitanceFarads;
}

std::unique_ptr<Genfun::AbsFunction> Capacitor::currentFromVoltage(
    Genfun::GENFUNCTION voltage, double) const {
    return detail::transformFunction(voltage, detail::Operation::Derivative,
                                     capacitanceFarads_);
}

std::unique_ptr<Genfun::AbsFunction> Capacitor::voltageFromCurrent(
    Genfun::GENFUNCTION current, double initialVoltage) const {
    return detail::transformFunction(current, detail::Operation::Integral,
                                     1.0 / capacitanceFarads_, initialVoltage);
}

  const std::vector<std::pair<double,double>> &  Capacitor::symbol() const {
    static std::vector<std::pair<double,double>> mySymbol{{-25.0, 0.0},
							  {-5.0, 0.0},
							  {100.0,100.0},
							  {-5.0, -12.0},
							  {-5.0, 12.0},
							  {100.0,100.0},
							  {5.0, -12.0},
							  {5.0, 12.0},
							  {100.0,100.0},
							  {5.0, 0.0},
							  {25.0,0.0}};

    return mySymbol;
  }
  
  
} // namespace circuit

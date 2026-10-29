#include "Circuit/Inductor.h"
#include "ConstitutiveFunction.h"

#include <cmath>
#include <stdexcept>
#include <utility>

namespace circuit {

Inductor::Inductor(std::string name, double inductanceHenrys)
    : CircuitElement(std::move(name), 2), inductanceHenrys_(inductanceHenrys) {
    if (!std::isfinite(inductanceHenrys_) || inductanceHenrys_ <= 0.0) {
        throw std::invalid_argument("Inductance must be finite and positive");
    }
}

std::string Inductor::type() const  {
    return "Inductor";
}

double Inductor::inductanceHenrys() const  {
    return inductanceHenrys_;
}

double Inductor::currentDerivativeForVoltageDrop(double voltageDrop) const  {
    return -voltageDrop / inductanceHenrys_;
}

double Inductor::currentDerivativeForVoltageDrop(double voltageDrop,
                                                 double inductanceHenrys) {
    if (!std::isfinite(inductanceHenrys) || inductanceHenrys <= 0.0) {
        throw std::invalid_argument("Inductance must be finite and positive");
    }
    return -voltageDrop / inductanceHenrys;
}

std::unique_ptr<Genfun::AbsFunction> Inductor::currentFromVoltage(
    Genfun::GENFUNCTION voltage, double initialCurrent) const {
    return detail::transformFunction(voltage, detail::Operation::Integral,
                                     -1.0 / inductanceHenrys_, initialCurrent);
}

std::unique_ptr<Genfun::AbsFunction> Inductor::voltageFromCurrent(
    Genfun::GENFUNCTION current, double) const {
    return detail::transformFunction(current, detail::Operation::Derivative,
                                     -inductanceHenrys_);
}

  const std::vector<std::pair<double,double>> &Inductor::symbol() const {
    static std::vector<std::pair<double,double>> mySymbol{{-25.0, 0.0},
							  {-17.0, 0.0},
							  {100.0,100.0},
							  {15.0, 0.0},
							  {25.0, 0.0},
							  {100.0,100.0},
							  {-17.0, 0.0},
							  {-17.0, -18.0},
							  {-9.0, -18.0},
							  {-9.0, 0.0},
							  {-9.0, -18.0},
							  {-1.0, -18.0},
							  {-1.0, 0.0},
							  {-1.0, -18.0},
							  {7.0, -18.0},
							  {7.0, 0.0},
							  {7.0, -18.0},
							  {15.0, -18.0},
							  {15.0, 0.0},
							  {300.0,300.0}};
    return mySymbol;
  }
  
} // namespace circuit

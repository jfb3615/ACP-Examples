#include "Circuit/ACVoltageSource.h"

#include <cmath>
#include <memory>
#include <stdexcept>
#include <utility>
#include <numbers>

namespace circuit {
namespace {

class SourceWaveformFunction final : public Genfun::AbsFunction {
public:
    using Genfun::AbsFunction::operator();

    SourceWaveformFunction(double amplitude, double frequency, double phase)
        : amplitude_(amplitude), frequency_(frequency), phase_(phase) {}

    double operator()(double time) const override {
        return waveformAt(time, amplitude_, frequency_, phase_);
    }

    double operator()(const Genfun::Argument& argument) const override {
        if (argument.size() != 1) {
            throw std::invalid_argument("AC source waveform expects one time argument");
        }
        return (*this)(argument.front());
    }

    unsigned int dimensionality() const override {
        return 1;
    }

    SourceWaveformFunction* clone() const override {
        return new SourceWaveformFunction(*this);
    }
    static double waveformAt(double timeSeconds, double peakVoltageVolts,double frequencyHertz, double phaseRadians) ;
  
private:
    double amplitude_;
    double frequency_;
    double phase_;
};

  double SourceWaveformFunction::waveformAt(double timeSeconds, double peakVoltageVolts,
					    double frequencyHertz, double phaseRadians)  {
    
    return peakVoltageVolts * std::sin(2.0 * std::numbers::pi * frequencyHertz * timeSeconds +
                                       phaseRadians);
  }

  
} // namespace

ACVoltageSource::ACVoltageSource(std::string name, double peakVoltageVolts,
                                 double frequencyHertz, double phaseRadians)
    : CircuitElement(std::move(name), 2),
      peakVoltageVolts_(peakVoltageVolts),
      frequencyHertz_(frequencyHertz),
      phaseRadians_(phaseRadians) {
    if (!std::isfinite(peakVoltageVolts_) || peakVoltageVolts_ < 0.0) {
        throw std::invalid_argument("AC source peak voltage must be finite and non-negative");
    }
    if (!std::isfinite(frequencyHertz_) || frequencyHertz_ <= 0.0) {
        throw std::invalid_argument("AC source frequency must be finite and positive");
    }
    if (!std::isfinite(phaseRadians_)) {
        throw std::invalid_argument("AC source phase must be finite");
    }
}

std::string ACVoltageSource::type() const  {
    return "ACVoltageSource";
}

double ACVoltageSource::peakVoltageVolts() const  {
    return peakVoltageVolts_;
}

double ACVoltageSource::frequencyHertz() const  {
    return frequencyHertz_;
}

double ACVoltageSource::phaseRadians() const  {
    return phaseRadians_;
}

double ACVoltageSource::voltageAt(double timeSeconds) const  {
  return SourceWaveformFunction::waveformAt(timeSeconds, peakVoltageVolts_, frequencyHertz_, phaseRadians_);
}


std::unique_ptr<Genfun::AbsFunction> ACVoltageSource::currentFromVoltage(Genfun::GENFUNCTION, double) const {
  return {};
}

std::unique_ptr<Genfun::AbsFunction> ACVoltageSource::voltageFromCurrent(
    Genfun::GENFUNCTION, double) const {
    return std::make_unique<SourceWaveformFunction>(
        peakVoltageVolts_, frequencyHertz_, phaseRadians_);
}


const std::vector<std::pair<double,double>> &ACVoltageSource::symbol() const {
  static std::vector<std::pair<double,double>> mySymbol{{-25.0, 0.0},
							{-15.0, 0.0},
							{100.0,100.0},
							{-15.0, 0.0},
							{25.0, 0.0},
							{100.0,100.0},
							{0.0,0.0},
							{15.0,15.0},
							{200.0,200.0},
							{-9.0, 0.0},
							{-5.0, -7.0},
							{-2.0, -7.0},
							{0.0, 0.0},
							{2.0, 7.0},
							{5.0, 7.0},
							{9.0, 0.0},
							{300.0,300.0}};
    return mySymbol;
  }
  

} // namespace circuit

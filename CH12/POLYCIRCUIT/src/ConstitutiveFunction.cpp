#include "ConstitutiveFunction.h"

#include <QatGenericFunctions/Argument.h>

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <utility>

namespace circuit::detail {
namespace {

class TransformedFunction final : public Genfun::AbsFunction {
public:
    using Genfun::AbsFunction::operator();

    TransformedFunction(Genfun::GENFUNCTION function, Operation operation,
                        double scale, double offset)
        : function_(function.clone()), operation_(operation), scale_(scale),
          offset_(offset) {
        if (function_->dimensionality() != 1) {
            throw std::invalid_argument(
                "Constitutive function transforms require a one-dimensional function");
        }
    }

    double operator()(double argument) const override {
        if (operation_ == Operation::Scale) {
            return scale_ * (*function_)(argument) + offset_;
        }
        if (operation_ == Operation::Derivative) {
            const double step = std::cbrt(std::numeric_limits<double>::epsilon()) *
                                std::max(1.0, std::abs(argument));
            return scale_ * ((*function_)(argument + step) -
                             (*function_)(argument - step)) /
                       (2.0 * step) +
                   offset_;
        }

        constexpr int intervalCount = 256;
        const double step = argument / intervalCount;
        double sum = (*function_)(0.0) + (*function_)(argument);
        for (int i = 1; i < intervalCount; ++i) {
            sum += (i % 2 == 0 ? 2.0 : 4.0) *
                   (*function_)(step * static_cast<double>(i));
        }
        return offset_ + scale_ * step * sum / 3.0;
    }

    double operator()(const Genfun::Argument& argument) const override {
        if (argument.size() != 1) {
            throw std::invalid_argument(
                "Constitutive function transforms require one argument");
        }
        return (*this)(argument.front());
    }

    unsigned int dimensionality() const override {
        return 1;
    }

    TransformedFunction* clone() const override {
        return new TransformedFunction(*this);
    }

private:
    std::shared_ptr<const Genfun::AbsFunction> function_;
    Operation operation_;
    double scale_;
    double offset_;
};

} // namespace

std::unique_ptr<Genfun::AbsFunction> transformFunction(
    Genfun::GENFUNCTION function, Operation operation, double scale,
    double offset) {
    return std::make_unique<TransformedFunction>(function, operation, scale, offset);
}

} // namespace circuit::detail

#pragma once

#include <QatGenericFunctions/AbsFunction.h>

#include <memory>

namespace circuit::detail {

enum class Operation {
    Scale,
    Derivative,
    Integral
};

std::unique_ptr<Genfun::AbsFunction> transformFunction(
    Genfun::GENFUNCTION function, Operation operation, double scale,
    double offset = 0.0);

} // namespace circuit::detail

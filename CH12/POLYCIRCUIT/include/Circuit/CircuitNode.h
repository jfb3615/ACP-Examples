#pragma once

#include <cstdint>

namespace circuit {

struct Point {
    double x{};
    double y{};
};

class CircuitNode {
public:
    using Id = std::uint64_t;

    CircuitNode(Id id, Point position = {});

    Id id() const ;
    Point position() const ;
    void setPosition(Point position) ;

private:
    Id id_;
    Point position_;
};

} // namespace circuit

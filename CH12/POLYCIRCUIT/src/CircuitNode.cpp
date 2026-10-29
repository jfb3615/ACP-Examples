#include "Circuit/CircuitNode.h"

namespace circuit {

CircuitNode::CircuitNode(Id id, Point position)
    : id_(id), position_(position) {}

CircuitNode::Id CircuitNode::id() const  {
    return id_;
}

Point CircuitNode::position() const  {
    return position_;
}

void CircuitNode::setPosition(Point position)  {
    position_ = position;
}

} // namespace circuit

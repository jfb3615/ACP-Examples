#pragma once

#include "Circuit/Circuit.h"

#include <QGraphicsView>

class QGraphicsScene;

namespace circuit::visualization {

void populateScene(const Circuit& circuit, QGraphicsScene& scene);

class CircuitView final : public QGraphicsView {
public:
    explicit CircuitView(const Circuit& circuit);
};

} // namespace circuit::visualization

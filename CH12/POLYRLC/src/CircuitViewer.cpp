#include "CircuitViewer.h"

#include "Circuit/ACVoltageSource.h"
#include "Circuit/Capacitor.h"
#include "Circuit/Inductor.h"
#include "Circuit/Resistor.h"

#include <QToolBar>
#include <QAction>
#include <QGraphicsEllipseItem>
#include <QGraphicsItem>
#include <QGraphicsLineItem>
#include <QGraphicsScene>
#include <QPainterPath>
#include <QPainter>
#include <QPen>
#include <QPolygonF>
#include <QPointF>
#include <QRectF>
#include <QBrush>
#include <QLineF>
#include <QApplication>
#include <cmath>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>

namespace circuit::visualization {
namespace {

constexpr qreal symbolHalfLength = 25.0;
constexpr qreal nodeRadius = 4.0;
constexpr qreal pi = 3.14159265358979323846;

  class CircuitSymbolItem final : public QGraphicsItem {
  public:
    
    CircuitSymbolItem(const CircuitElement* element, QString name)
      : element_(element), name_(std::move(name)) {}
    
    QRectF boundingRect() const override {
      return {-symbolHalfLength - 8.0, -34.0,
	      (symbolHalfLength + 8.0) * 2.0, 52.0};
    }

    
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override {
      painter->setRenderHint(QPainter::Antialiasing);
      painter->setPen(QPen(Qt::black, 2.0));
      painter->setBrush(Qt::white);

      const std::vector<std::pair<double,double>> & symbol=element_->symbol();
      std::unique_ptr<QPolygonF> zz=std::make_unique<QPolygonF>();
      for (auto & [x,y] : symbol) {
	if (x==100.0 && y==100.0) {
	  painter->drawPolyline(*zz);
	  zz.reset(new QPolygonF);
	}
	else if (x==200.0 && y==200.0) {
	  painter->drawEllipse(QPointF((*zz)[0].x(),(*zz)[0].y()),(*zz)[1].x(),(*zz)[1].y());
	  zz.reset(new QPolygonF);
	}
	else if (x==300.0 && y==300.0) {
	  QPainterPath path;
	  path.moveTo((*zz)[0].x(),(*zz)[0].y());
	  int i=1;
	  while (i<zz->size()) {
	    double x1=(*zz)[i].x();
	    double x2=(*zz)[i].y();
	    double x3=(*zz)[i+1].x();
	    double x4=(*zz)[i+1].y();
	    double x5=(*zz)[i+2].x();
	    double x6=(*zz)[i+2].y();
	    path.cubicTo(x1,x2,x3,x4,x5,x6);
	    i+=3;
	  }
	  painter->drawPath(path);
	  zz.reset(new QPolygonF);
	}
	else {
	  (*zz) << QPointF(x,y);
	}
      }
      painter->drawPolyline(*zz);
      
   
      painter->setPen(Qt::black);
      painter->drawText(QRectF(-35.0, -34.0, 70.0, 16.0),
			Qt::AlignCenter, name_);
    }

private:
  const CircuitElement *element_;
  QString name_;
};

QPointF toScenePoint(Point point) {
    return {point.x, point.y};
}

} // namespace

void populateScene(const Circuit& circuit, QGraphicsScene& scene) {
    scene.clear();
    std::unordered_map<CircuitNode::Id, QPointF> nodePositions;
    nodePositions.reserve(circuit.nodes().size());
    std::size_t capacitorIndex = 0;
    std::size_t inductorIndex = 0;

    for (const auto& node : circuit.nodes()) {
        const auto position = toScenePoint(node->position());
        nodePositions.emplace(node->id(), position);
    }

    for (const auto& element : circuit.elements()) {
        if (element->terminalCount() != 2 || !element->terminal(0) ||
            !element->terminal(1)) {
            throw std::invalid_argument(
                "The circuit viewer requires each element to connect to two nodes");
        }

        QString label = QString::fromStdString(element->name());

        const auto first = nodePositions.find(element->terminal(0)->id());
        const auto second = nodePositions.find(element->terminal(1)->id());
        if (first == nodePositions.end() || second == nodePositions.end()) {
            throw std::invalid_argument(
                "An element terminal references a node that is not in the circuit");
        }

        const QPointF delta = second->second - first->second;
        const qreal distance = std::hypot(delta.x(), delta.y());
        if (distance == 0.0) {
            throw std::invalid_argument(
                "The circuit viewer cannot render an element connected to the same position twice");
        }

        const QPointF direction = delta / distance;
        const QPointF center = (first->second + second->second) / 2.0;
        const QPointF offset = direction * symbolHalfLength;

        auto* firstWire = scene.addLine(QLineF(first->second, center - offset),
                                        QPen(Qt::black, 1.5));
        firstWire->setZValue(-1.0);
        auto* secondWire = scene.addLine(QLineF(center + offset, second->second),
                                         QPen(Qt::black, 1.5));
        secondWire->setZValue(-1.0);

        auto* symbol = new CircuitSymbolItem(element.get(), label);
        symbol->setData(0, label);
        symbol->setPos(center);
        symbol->setRotation(std::atan2(delta.y(), delta.x()) * 180.0 / pi);
        symbol->setZValue(1.0);
        scene.addItem(symbol);
    }

    for (const auto& node : circuit.nodes()) {
        const QPointF position = nodePositions.at(node->id());
        auto* dot = scene.addEllipse(
            QRectF(position.x() - nodeRadius, position.y() - nodeRadius,
                   nodeRadius * 2.0, nodeRadius * 2.0),
            QPen(Qt::black, 1.0), QBrush(Qt::black));
        dot->setZValue(2.0);
    }

    scene.setSceneRect(scene.itemsBoundingRect().adjusted(-30.0, -30.0, 30.0, 30.0));
}

CircuitView::CircuitView(const Circuit& circuit)
    : QGraphicsView() {

  QAction  *quitAction=addAction("Quit");
  quitAction->setShortcut(QKeySequence("q"));
  QObject::connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);

  auto* circuitScene = new QGraphicsScene(this);
  setScene(circuitScene);
  populateScene(circuit, *circuitScene);
  setWindowTitle(QStringLiteral("Circuit Viewer"));
  setRenderHint(QPainter::Antialiasing);
  setDragMode(QGraphicsView::ScrollHandDrag);
  fitInView(circuitScene->sceneRect(), Qt::KeepAspectRatio);
  resize(800, 600);
}
  
} // namespace circuit::visualization

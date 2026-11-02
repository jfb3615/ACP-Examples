#include "CircuitViewer.h"
#include "Circuit/Path.h"
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
#include <numbers>

namespace circuit::visualization {
  constexpr qreal symbolHalfLength = 25.0;
  constexpr qreal nodeRadius = 4.0;
  
  class CircuitSymbolItem final : public QGraphicsItem {
  public:
    
    CircuitSymbolItem(const CircuitElement* element, QString name)
      : element_(element), name_(std::move(name)) {}
    
    QRectF boundingRect() const override {
      return {-symbolHalfLength - 8.0, -34.0,
	      (symbolHalfLength + 8.0) * 2.0, 52.0};
    }
    
    void setPen(const QPen & pen) {
      pen_=pen;
    }
    
    const CircuitElement * getElement() const {
      return element_;
    }
    
    // Decode the symbol encodings and draw the circuit element:
    
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override {
      painter->setRenderHint(QPainter::Antialiasing);
      painter->setPen(QPen(Qt::black, 2.0));
      painter->setPen(pen_);
      painter->setBrush(Qt::white);
      
      const std::vector<std::pair<double,double>> & symbol=element_->symbol();
      const std::vector<std::pair<CircuitElement::SymbolSegmentType, size_t>> & symbolTypeAndNumber= element_->symbolTypeAndNumber();
      size_t cc{0};
      for (const auto & [type, num] : symbolTypeAndNumber) {
	if (type==CircuitElement::POLYLINE) {
	  QPolygonF zz;
	  for (int i=0; i<num; i++) {
	    zz << QPointF(symbol[cc+i].first,symbol[cc+i].second);
	  }
	  cc+=num;
	  painter->drawPolyline(zz);
	}
	else if (type==CircuitElement::ELLIPSE) {
	  painter->drawEllipse(QPointF(symbol[cc+0].first,symbol[cc+0].second),symbol[cc+1].first,symbol[cc+1].second);
	  cc+=2;
	}
	else if (type==CircuitElement::CUBIC) {
	  QPainterPath path;
	  for (int i=0;i<num;i++) {
	    
	    path.moveTo(symbol[cc+0].first,symbol[cc+0].second);
	    
	    double x1=symbol[cc+1].first;
	    double x2=symbol[cc+1].second;
	    double x3=symbol[cc+2].first;
	    double x4=symbol[cc+2].second;
	    double x5=symbol[cc+3].first;
	    double x6=symbol[cc+3].second;
	    path.cubicTo(x1,x2,x3,x4,x5,x6);
	    cc+=3;
	  }
	  painter->drawPath(path);	  }
	else {
	  throw std::runtime_error("Unknown circuit symbol type");
	}
      }
      
      painter->setPen(Qt::black);
      painter->drawText(QRectF(-35.0, -34.0, 70.0, 16.0),
			Qt::AlignCenter, name_);
    }
    
  private:
    const CircuitElement *element_;
    QString name_;
    QPen    pen_{Qt::black, 2};
  };


  
  QPointF toScenePoint(Point point) {
    return {point.x, point.y};
  }
  
  

  
  void populateScene(const Circuit& circuit, QGraphicsScene& scene,
		     PathMonger & monger,
		     std::vector<CircuitSymbolItem *> & symbolVector) {
    
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
      symbol->setRotation(std::atan2(delta.y(), delta.x()) * 180.0 / std::numbers::pi);
      symbol->setZValue(1.0);
      scene.addItem(symbol);
      symbolVector.push_back(symbol);
      
    }


    // Put black dots for circuit nodes:
    for (const auto& node : circuit.nodes()) {
      const QPointF position = nodePositions.at(node->id());
      auto* dot = scene.addEllipse( QRectF(position.x() - nodeRadius, position.y() - nodeRadius,
				    nodeRadius * 2.0, nodeRadius * 2.0),
				    QPen(Qt::black, 1.0), QBrush(Qt::black));
      dot->setZValue(2.0);
    }
    
    scene.setSceneRect(scene.itemsBoundingRect().adjusted(-30.0, -30.0, 30.0, 30.0));
  }


  // This class catches the "Next" action. The next action moves the viewer through the
  // list of independent closed paths found by the PathMonger, these are all supposed
  // to be independent but visual inspection will say for sure. 
  class SignalCatcher:public QObject {
    
    Q_OBJECT
    
  public:
    
    // Constructor:
    SignalCatcher(const PathMonger & monger, const std::vector<CircuitSymbolItem *>  & symbolVector) :
      symbolVector(symbolVector), monger(monger){;}

  public slots:

    // Update to the next path or if we are finished return to black. 
    void update() {
      pathNumber++;
      pathNumber %= (monger.getNumPaths()+1);
      for (CircuitSymbolItem  *item : symbolVector) {
	if (pathNumber==monger.getNumPaths()) {
	  item->setPen(QPen(Qt::black, 2));
	  pathNumber==-1;
	}
	else {
	  Path p=monger.getPath(pathNumber);
	  if (p.contains(item->getElement())) {
	    item->setPen(QPen(Qt::red,4));
	  }
	  else {
	    item->setPen(QPen(Qt::black, 2));
	  }
	}
	item->update();
      }
    }
    //
  private:
    
    int                                      pathNumber{-1};                    
    const PathMonger                       & monger;
    const std::vector<CircuitSymbolItem *> & symbolVector;
    
  };
  

  // This class inherits from QGraphicsView. It creates the scene, adds it to the view
  // and sets up user interactions.  
  CircuitView::CircuitView(const Circuit& circuit)
    : QGraphicsView() {
    
    // Arrange for the program to exit when the user hits the "q" key:   
    QAction  *quitAction=addAction("Quit");
    quitAction->setShortcut(QKeySequence("q"));
    QObject::connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);
    
    static std::vector<CircuitSymbolItem *> symbolVector;
    static PathMonger monger(circuit);
    static SignalCatcher signalCatcher(monger,symbolVector);

    // Arrange for the next independent loop be drawn when the user hits the "n" key:
    QAction  *nextAction=addAction("Next");
    nextAction->setShortcut(QKeySequence("n"));
    QObject::connect(nextAction, &QAction::triggered, &signalCatcher, &SignalCatcher::update);


    // Create the scene
    auto* circuitScene = new QGraphicsScene(this);

    // Add the scene to the viewer:
    setScene(circuitScene);
    
    //Put the circuit elements in the scene:
    populateScene(circuit, *circuitScene, monger, symbolVector);

    QGraphicsTextItem *text=circuitScene->addText(QString(" Type n to see next loop, q to exit"));

    text->moveBy(200,350);
    
    setWindowTitle(QStringLiteral("Circuit Viewer"));
    resize(800, 600);
  }
  
} // namespace circuit::visualization
#include "CircuitViewer.moc"

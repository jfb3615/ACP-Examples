#include "Circuit/ACVoltageSource.h"
#include "Circuit/Capacitor.h"
#include "Circuit/Circuit.h"
#include "Circuit/CircuitSimulator.h"
#include "Circuit/Inductor.h"
#include "Circuit/Resistor.h"
#include "Circuit/Wire.h"
#include <QApplication>
#include <QRectF>
#include <QTextEdit>
#include <QAction>
#include <QToolBar>
#include <QatGenericFunctions/AbsFunction.h>
#include <QatPlotWidgets/MultipleViewWindow.h>
#include <QatPlotWidgets/PlotView.h>
#include <QatPlotting/PlotStream.h>
#include <QatPlotting/PRectF.h>
#include <QatPlotting/PlotFunction1D.h>
#include <iostream>

//
// Test program to lay out and view a simple diagram.
// 
int main(int argc, char* argv[]) {
  using namespace circuit;

  // Model:
  Circuit model;

  // Nodes:
  CircuitNode & nodeA = model.addNode({0.0,     0.0});
  CircuitNode & nodeB = model.addNode({200.0,   0.0});
  CircuitNode & nodeC = model.addNode({400.0,   0.0});
  CircuitNode & nodeD = model.addNode({600.0,   0.0});
  CircuitNode & nodeE = model.addNode({0.0,     300.0});
  CircuitNode & nodeF = model.addNode({200.0,   300.0});
  CircuitNode & nodeG = model.addNode({400.0,   300.0});
  CircuitNode & nodeH = model.addNode({600.0,   300.0});

  // Elements:
  CircuitElement & source1    = model.addElement(std::make_unique<circuit::ACVoltageSource>("V1", 10.0, 50.0));
  CircuitElement & resistor1  = model.addElement(std::make_unique<circuit::Resistor>("R1", 100.0));
  CircuitElement & resistor2  = model.addElement(std::make_unique<circuit::Resistor>("R2", 200.0));
  CircuitElement & capacitor1 = model.addElement(std::make_unique<circuit::Capacitor>("C1", 100e-6));
  CircuitElement & inductor1 = model.addElement(std::make_unique<circuit::Inductor>("L1", 10e-3));

  // Wires
  CircuitElement & wire1 = model.addElement(std::make_unique<circuit::Wire>("W1"));
  CircuitElement & wire2 = model.addElement(std::make_unique<circuit::Wire>("W2"));
  CircuitElement & wire3 = model.addElement(std::make_unique<circuit::Wire>("W3"));
  CircuitElement & wire4 = model.addElement(std::make_unique<circuit::Wire>("W4"));
  CircuitElement & wire5 = model.addElement(std::make_unique<circuit::Wire>("W5"));

  // Connections: wires to nodes:
  wire1.connectTerminal(0,nodeB); wire1.connectTerminal(1,nodeC);
  wire2.connectTerminal(0,nodeC); wire2.connectTerminal(1,nodeD);
  wire3.connectTerminal(0,nodeE); wire3.connectTerminal(1,nodeF);
  wire4.connectTerminal(0,nodeF); wire4.connectTerminal(1,nodeG);
  wire5.connectTerminal(0,nodeG); wire5.connectTerminal(1,nodeH);

  // Connections::elements to nodes:
  source1.connectTerminal(0, nodeA);  source1.connectTerminal(1, nodeE);
  resistor1.connectTerminal(0, nodeA); resistor1.connectTerminal(1, nodeB);
  resistor2.connectTerminal(0, nodeD); resistor2.connectTerminal(1, nodeH);
  capacitor1.connectTerminal(0, nodeB); capacitor1.connectTerminal(1, nodeF);
  inductor1.connectTerminal(0, nodeC); inductor1.connectTerminal(1, nodeG);

  // Reduce. Absorb wires into nodes. (Should this be automatically done)? 
  model.reduce();

  // Simulate:
  circuit::CircuitSimulator simulator(model);

  // Prepare application, window, toolbar, quit button & shortcut:
  QApplication application(argc, argv);
  MultipleViewWindow window;
  window.setWindowTitle(QStringLiteral("RLC Circuit"));
  QToolBar *toolBar=window.addToolBar("Tools");
  QAction  *quitAction=toolBar->addAction("Quit");
  quitAction->setShortcut(QKeySequence("q"));
  QObject::connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);

  
  // PlotViews:: create and add to window:
  PlotView C1VoltageView({0.0,0.1,-1.0,1.0});
  PlotView L1CurrentView({0.0,0.1,-0.2,0.2});
  window.add(&C1VoltageView,"C1 Voltage");
  window.add(&L1CurrentView,"L1 Current");

  // PlotFunctions from simulator:  create and add to views
  PlotFunction1D plotC1V{*simulator.getCurrent(0)};
  PlotFunction1D plotL1I{*simulator.getCurrent(1)};
  C1VoltageView.add(&plotC1V);
  L1CurrentView.add(&plotL1I);

  // Label the plots:
  auto sz16=PlotStream::Size(16);
  PlotStream(C1VoltageView.titleTextEdit()) << sz16  << "Voltage (fake implementation)" << PlotStream::EndP();
  PlotStream(L1CurrentView.titleTextEdit()) << sz16  << "L1 Current(fake impementation)" << PlotStream::EndP();
  PlotStream(C1VoltageView.xLabelTextEdit()) << sz16  << "time [seconds]" << PlotStream::EndP();
  PlotStream(L1CurrentView.xLabelTextEdit()) << sz16  << "time [seconds]" << PlotStream::EndP();
  PlotStream(C1VoltageView.yLabelTextEdit()) << sz16  << "V [Volts]"   << PlotStream::EndP();
  PlotStream(L1CurrentView.yLabelTextEdit()) << sz16  << "I [Amperes]" << PlotStream::EndP();

  // Interact:
  window.show();
  return application.exec();
}

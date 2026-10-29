#include "Circuit/ACVoltageSource.h"
#include "Circuit/Capacitor.h"
#include "Circuit/Circuit.h"
#include "Circuit/Inductor.h"
#include "Circuit/Resistor.h"
#include "Circuit/Wire.h"
#include "CircuitViewer.h"

#include <QApplication>
#include <QRectF>
#include <QTextEdit>

#include <QatGenericFunctions/AbsFunction.h>
#include <QatPlotWidgets/MultipleViewWindow.h>
#include <QatPlotWidgets/PlotView.h>
#include <QatPlotting/PRectF.h>
#include <QatPlotting/PlotFunction1D.h>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace {

int fail(const std::string& message) {
    std::cerr << "Test failed: " << message << '\n';
    return 1;
}

void configurePlot(PlotView& view, const QString& title, const QString& yAxisLabel) {
    view.setWindowTitle(title);
    view.titleTextEdit()->setPlainText(title);
    view.xLabelTextEdit()->setPlainText(QStringLiteral("Time (s)"));
    view.yLabelTextEdit()->setPlainText(yAxisLabel);
    view.resize(800, 500);
}

std::size_t root(std::vector<std::size_t>& parents, std::size_t index) {
    while (parents[index] != index) {
        parents[index] = parents[parents[index]];
        index = parents[index];
    }
    return index;
}

} // namespace

int main(int argc, char* argv[]) {
    bool showViewer = true;

    circuit::Circuit model;
    auto& nodeA = model.addNode({0.0,     0.0});
    auto& nodeB = model.addNode({200.0,   0.0});
    auto& nodeC = model.addNode({400.0,   0.0});
    auto& nodeD = model.addNode({600.0,   0.0});
    auto& nodeE = model.addNode({0.0,     300.0});
    auto& nodeF = model.addNode({200.0,   300.0});
    auto& nodeG = model.addNode({400.0,   300.0});
    auto& nodeH = model.addNode({600.0,   300.0});

 


    auto& source1 = static_cast<circuit::ACVoltageSource&>(
        model.addElement(std::make_unique<circuit::ACVoltageSource>("V1", 10.0, 50.0)));

    auto& resistor1 = static_cast<circuit::Resistor&>(
        model.addElement(std::make_unique<circuit::Resistor>("R1", 100.0)));

    auto& resistor2 = static_cast<circuit::Resistor&>(
        model.addElement(std::make_unique<circuit::Resistor>("R2", 200.0)));

    auto& capacitor1 = static_cast<circuit::Capacitor&>(
        model.addElement(
            std::make_unique<circuit::Capacitor>("C1", 100e-6)));

     auto& inductor1 = static_cast<circuit::Inductor&>(
        model.addElement(
            std::make_unique<circuit::Inductor>("L1", 10e-3)));

     auto& wire1 = static_cast<circuit::Inductor&>(model.addElement(std::make_unique<circuit::Wire>("W1")));
     auto& wire2 = static_cast<circuit::Inductor&>(model.addElement(std::make_unique<circuit::Wire>("W2")));
     auto& wire3 = static_cast<circuit::Inductor&>(model.addElement(std::make_unique<circuit::Wire>("W3")));
     auto& wire4 = static_cast<circuit::Inductor&>(model.addElement(std::make_unique<circuit::Wire>("W4")));
     auto& wire5 = static_cast<circuit::Inductor&>(model.addElement(std::make_unique<circuit::Wire>("W5")));



     wire1.connectTerminal(0,nodeB); wire1.connectTerminal(1,nodeC);
     wire2.connectTerminal(0,nodeC); wire2.connectTerminal(1,nodeD);
     wire3.connectTerminal(0,nodeE); wire3.connectTerminal(1,nodeF);
     wire4.connectTerminal(0,nodeF); wire4.connectTerminal(1,nodeG);
     wire5.connectTerminal(0,nodeG); wire5.connectTerminal(1,nodeH);
    
     source1.connectTerminal(0, nodeA);  source1.connectTerminal(1, nodeE);
     
     resistor1.connectTerminal(0, nodeA); resistor1.connectTerminal(1, nodeB);
     resistor2.connectTerminal(0, nodeD); resistor2.connectTerminal(1, nodeH);


    
    capacitor1.connectTerminal(0, nodeB); capacitor1.connectTerminal(1, nodeF);


    inductor1.connectTerminal(0, nodeC); inductor1.connectTerminal(1, nodeG);

    std::unordered_map<const circuit::CircuitNode*, std::size_t> indices;
    indices.reserve(model.nodes().size());
    for (std::size_t i = 0; i < model.nodes().size(); ++i) {
        indices.emplace(model.nodes()[i].get(), i);
    }
    std::vector<std::size_t> parents(model.nodes().size());
    for (std::size_t i = 0; i < parents.size(); ++i) {
        parents[i] = i;
    }

    std::size_t independentLoops = 0;
    for (const auto& element : model.elements()) {
        if (element->terminalCount() != 2 || !element->terminal(0) ||
            !element->terminal(1)) {
            return fail("each component must connect to exactly two nodes");
        }
        std::size_t first = root(parents, indices.at(element->terminal(0)));
        std::size_t second = root(parents, indices.at(element->terminal(1)));
        if (first == second) {
            ++independentLoops;
        } else {
            parents[first] = second;
        }
    }
    std::cout << "There are " << independentLoops << " independent Loops" << std::endl;
    if (independentLoops != 3) {
       return fail("expected four independent loops in the expanded topology");
    }
    

    if (showViewer) {
        int qtArgc = 1;
        char* qtArgv[] = {argv[0], nullptr};
        QApplication application(qtArgc, qtArgv);
        circuit::visualization::CircuitView viewer(model);
        viewer.setWindowTitle(QStringLiteral("Complex Circuit Topology"));
        viewer.show();
        return application.exec();
    }



    return 0;
}

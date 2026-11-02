#include "Circuit/CircuitSimulator.h"

#include <QatGenericFunctions/AbsFunction.h>
#include <QatGenericFunctions/AdaptiveRKStepper.h>
#include <QatGenericFunctions/RKIntegrator.h>
#include <QatGenericFunctions/Variable.h>
#include "Eigen/Dense"
#include <algorithm>
#include <cmath>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include "CircuitSystem.h"
#include "CircuitEquation.h"
namespace circuit {
namespace {



  
std::size_t nodeColumn(const CircuitSystem& system, std::size_t node) {
    if (node == system.groundNode) {
        return system.nodeCount;
    }
    return node < system.groundNode ? node : node - 1;
}

  void addBranchKcl(Eigen::MatrixXd & matrix,
		    const CircuitSystem& system, const Branch& branch,
		    std::size_t column, double scale = 1.0) {
    const std::size_t first = nodeColumn(system, branch.firstNode);
    const std::size_t second = nodeColumn(system, branch.secondNode);
    if (first < system.nodeCount) {
      matrix(first,column) += scale;
    }
    if (second < system.nodeCount) {
      matrix(second,column) -= scale;
    }
  }

CircuitSystem scanCircuit(const Circuit& circuit) {
    if (circuit.nodes().empty()) throw std::invalid_argument("Circuit simulation requires at least one node");

    CircuitSystem system;
    system.nodeCount = circuit.nodes().size();
    system.groundNode = 0;
    std::unordered_map<const CircuitNode*, std::size_t> nodeIndices;
    nodeIndices.reserve(system.nodeCount);
    for (std::size_t i = 0; i < system.nodeCount; ++i) nodeIndices.emplace(circuit.nodes()[i].get(), i);

    const auto makeBranch = [&nodeIndices](const CircuitElement& element) {
        const CircuitNode* first = element.terminal(0);
        const CircuitNode* second = element.terminal(1);
        const auto firstFound = nodeIndices.find(first);
        const auto secondFound = nodeIndices.find(second);
        if (firstFound == nodeIndices.end() || secondFound == nodeIndices.end()) {
            throw std::invalid_argument(
                "A component terminal references a node outside this circuit");
        }
        if (first == second) {
            throw std::invalid_argument(
                "A component cannot connect both terminals to the same node");
        }
        return Branch{firstFound->second, secondFound->second};
    };

    for (const auto& element : circuit.elements()) {
        if (element->terminalCount() != 2 || !element->terminal(0) ||
            !element->terminal(1)) {
            throw std::invalid_argument(
                "Circuit simulation requires every component to connect to two nodes");
        }
        const Branch branch = makeBranch(*element);

        const std::string type = element->type();
        if (type == "Resistor" || type == "Wire") {
            Genfun::Variable time;
            const auto response = element->currentFromVoltage(time);
            const double conductance = (*response)(1.0);
            if (!std::isfinite(conductance) || conductance <= 0.0) {
                throw std::invalid_argument(
                    "Resistor current-from-voltage law must have positive conductance");
            }
            system.resistors.push_back({branch, 1.0 / conductance});
        } else if (type == "Capacitor") {
            Genfun::Variable time;
            const auto response = element->currentFromVoltage(time);
            const double capacitance = (*response)(0.25);
            if (!std::isfinite(capacitance) || capacitance <= 0.0) {
                throw std::invalid_argument(
                    "Capacitor current-from-voltage law must have positive capacitance");
            }
            const std::size_t stateIndex = system.stateNames.size();
            system.capacitors.push_back({branch, capacitance, stateIndex});
            system.stateNames.push_back(element->name() + " voltage (V)");
        } else if (type == "Inductor") {
            Genfun::Variable time;
            const auto response = element->voltageFromCurrent(time);
            const double inductance = -(*response)(0.25);
            if (!std::isfinite(inductance) || inductance <= 0.0) {
                throw std::invalid_argument(
                    "Inductor voltage-from-current law must have positive inductance");
            }
            const std::size_t stateIndex = system.stateNames.size();
            system.inductors.push_back({branch, inductance, stateIndex});
            system.stateNames.push_back(element->name() + " current (A)");
        } else if (type == "ACVoltageSource") {
            Genfun::Variable time;
            system.sources.push_back(
                {branch, element->voltageFromCurrent(time)});
        } else {
            throw std::invalid_argument("Unsupported circuit element: " + type);
        }
    }

    return system;
}

std::vector<double> repeatedInitialValue(const Circuit& circuit,
                                         const std::string& componentType,
                                         double value) {
    std::size_t count = 0;
    for (const auto& element : circuit.elements()) {
        if (componentType == element->type()) {
            ++count;
        }
    }
    return std::vector<double>(count, value);
}

std::vector<double> evaluateDerivatives(const CircuitSystem& system,
                                        const Genfun::Argument& state) {
    std::lock_guard<std::mutex> lock(system.cache->mutex);
    if (system.cache->valid && system.cache->state == state) {
        return system.cache->derivatives;
    }
    const std::size_t stateCount = system.stateNames.size();
    if (state.size() != stateCount + 1) {
        throw std::invalid_argument("Circuit ODE state has an unexpected dimension");
    }
    const std::size_t timeIndex = stateCount;
    const std::size_t nodeVoltageCount = system.nodeCount - 1;
    const std::size_t sourceCurrentOffset = nodeVoltageCount;
    const std::size_t capacitorDerivativeOffset =
        sourceCurrentOffset + system.sources.size();
    const std::size_t inductorDerivativeOffset =
        capacitorDerivativeOffset + system.capacitors.size();
    const std::size_t equationCount =
        nodeVoltageCount + system.sources.size() + system.capacitors.size() +
        system.inductors.size();

    Eigen::MatrixXd matrix=Eigen::MatrixXd::Zero(equationCount,equationCount);
    Eigen::VectorXd rightHandSide=Eigen::VectorXd::Zero(equationCount);

    for (const auto& resistor : system.resistors) {
        const double conductance = 1.0 / resistor.resistance;
        const std::size_t first = nodeColumn(system, resistor.branch.firstNode);
        const std::size_t second = nodeColumn(system, resistor.branch.secondNode);
        if (first < nodeVoltageCount) {
	  matrix(first,first) += conductance;
        }
        if (second < nodeVoltageCount) {
	  matrix(second,second) += conductance;
        }
        if (first < nodeVoltageCount && second < nodeVoltageCount) {
	  matrix(first,second) -= conductance;
	  matrix(second,first) -= conductance;
        }
    }

    for (std::size_t i = 0; i < system.sources.size(); ++i) {
        const auto& source = system.sources[i];
        const std::size_t sourceColumn = sourceCurrentOffset + i;
        addBranchKcl(matrix, system, source.branch, sourceColumn);
        const std::size_t constraintRow = nodeVoltageCount + i;
        const std::size_t first = nodeColumn(system, source.branch.firstNode);
        const std::size_t second = nodeColumn(system, source.branch.secondNode);
        if (first < nodeVoltageCount) {
	  matrix(constraintRow,first) += 1.0;
        }
        if (second < nodeVoltageCount) {
	  matrix(constraintRow,second) -= 1.0;
        }
        rightHandSide(constraintRow) =
            (*source.waveform)(state[timeIndex]);
    }

    for (std::size_t i = 0; i < system.capacitors.size(); ++i) {
        const auto& capacitor = system.capacitors[i];
        const std::size_t derivativeColumn = capacitorDerivativeOffset + i;
        const std::size_t constraintRow =
            nodeVoltageCount + system.sources.size() + i;
        addBranchKcl(matrix, system, capacitor.branch, derivativeColumn,
                     capacitor.capacitance);

        const std::size_t first = nodeColumn(system, capacitor.branch.firstNode);
        const std::size_t second = nodeColumn(system, capacitor.branch.secondNode);
        if (first < nodeVoltageCount) {
	  matrix(constraintRow,first) += 1.0;
        }
        if (second < nodeVoltageCount) {
	  matrix(constraintRow,second) -= 1.0;
        }
        rightHandSide(constraintRow) = state[capacitor.stateIndex];
    }

    for (std::size_t i = 0; i < system.inductors.size(); ++i) {
        const auto& inductor = system.inductors[i];
        const std::size_t derivativeColumn = inductorDerivativeOffset + i;
        const std::size_t constraintRow = nodeVoltageCount + system.sources.size() +
                                          system.capacitors.size() + i;
        const std::size_t firstRow = nodeColumn(system, inductor.branch.firstNode);
        const std::size_t secondRow = nodeColumn(system, inductor.branch.secondNode);
        if (firstRow < nodeVoltageCount) {
	  rightHandSide(firstRow) += state[inductor.stateIndex];
        }
        if (secondRow < nodeVoltageCount) {
	  rightHandSide(secondRow) -= state[inductor.stateIndex];
        }

        const std::size_t first = nodeColumn(system, inductor.branch.firstNode);
        const std::size_t second = nodeColumn(system, inductor.branch.secondNode);
        if (first < nodeVoltageCount) {
	  matrix(constraintRow,first) += 1.0;
        }
        if (second < nodeVoltageCount) {
	  matrix(constraintRow,second) -= 1.0;
        }
        // The inductor state current is positive terminal-1-to-0, so its
        // terminal-0-to-1 voltage follows V=-L*dI/dt.
        matrix(constraintRow,derivativeColumn) = inductor.inductance;

    }

    const Eigen::VectorXd solution =matrix.colPivHouseholderQr().solve(rightHandSide);

    std::vector<double> derivatives(stateCount + 1, 0.0);
    for (std::size_t i = 0; i < system.capacitors.size(); ++i) {
        derivatives[system.capacitors[i].stateIndex] = solution(capacitorDerivativeOffset + i);
    }
    for (std::size_t i = 0; i < system.inductors.size(); ++i) {
        derivatives[system.inductors[i].stateIndex] = solution(inductorDerivativeOffset + i);
    }
    derivatives[stateCount] = 1.0;
    system.cache->state = state;
    system.cache->derivatives = derivatives;
    system.cache->valid = true;
    return derivatives;
}

} // namespace

class CircuitSimulator::Clockwork {
public:
    Clockwork(const Circuit& circuit, std::vector<double> initialCapacitorVoltages,
         std::vector<double> initialInductorCurrents)
        : system(std::make_shared<CircuitSystem>(scanCircuit(circuit))) {
        if (initialCapacitorVoltages.empty()) {
            initialCapacitorVoltages.resize(system->capacitors.size(), 0.0);
        }
        if (initialInductorCurrents.empty()) {
            initialInductorCurrents.resize(system->inductors.size(), 0.0);
        }
        if (initialCapacitorVoltages.size() != system->capacitors.size() ||
            initialInductorCurrents.size() != system->inductors.size()) {
            throw std::invalid_argument(
                "Initial conditions must provide one value per capacitor and inductor");
        }

        startingValues.resize(system->stateNames.size() + 1, 0.0);
        for (std::size_t i = 0; i < system->capacitors.size(); ++i) {
            if (!std::isfinite(initialCapacitorVoltages[i])) {
                throw std::invalid_argument("Initial capacitor voltages must be finite");
            }
            startingValues[system->capacitors[i].stateIndex] =
                initialCapacitorVoltages[i];
        }
        for (std::size_t i = 0; i < system->inductors.size(); ++i) {
            if (!std::isfinite(initialInductorCurrents[i])) {
                throw std::invalid_argument("Initial inductor currents must be finite");
            }
            startingValues[system->inductors[i].stateIndex] =
                initialInductorCurrents[i];
        }

        equations.reserve(startingValues.size());
        for (std::size_t i = 0; i < startingValues.size(); ++i) {
            equations.push_back(std::make_unique<CircuitEquation>(system, i));
            integrator.addDiffEquation(equations.back().get(),
                                       i < system->stateNames.size()
                                           ? system->stateNames[i]
                                           : "simulation time",
                                       startingValues[i]);
        }
    }

    std::shared_ptr<CircuitSystem> system;
    std::vector<std::unique_ptr<CircuitEquation>> equations;
    std::vector<double> startingValues;
    Genfun::AdaptiveRKStepper stepper;
    Genfun::RKIntegrator integrator{&stepper};
};

CircuitSimulator::CircuitSimulator(const Circuit& circuit,
                                   double initialCapacitorVoltage,
                                   double initialInductorCurrent)
    : CircuitSimulator(circuit,
                       repeatedInitialValue(circuit, "Capacitor",initialCapacitorVoltage),
                       repeatedInitialValue(circuit, "Inductor", initialInductorCurrent)) {}

  CircuitSimulator::CircuitSimulator(
				     const Circuit& circuit, std::vector<double> initialCapacitorVoltages,
				     std::vector<double> initialInductorCurrents)
    : cw_(std::make_unique<Clockwork>(circuit, std::move(initialCapacitorVoltages),std::move(initialInductorCurrents))) {}


  CircuitSimulator::~CircuitSimulator()=default;

const std::vector<std::string>& CircuitSimulator::independentVariables() const  {
    return cw_->system->stateNames;
}

  std::size_t CircuitSimulator::numStateVariables() const  {
    return cw_->system->stateNames.size();
  }

  Genfun::GENFUNCTION CircuitSimulator::stateFunction(std::size_t index) const {
    if (index >= numStateVariables()) {
        throw std::out_of_range("Circuit simulator state index is out of range");
    }
    return *cw_->integrator.getFunction(static_cast<unsigned int>(index));
}

} // namespace circuit

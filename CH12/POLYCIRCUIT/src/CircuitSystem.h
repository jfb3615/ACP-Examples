#pragma once
#include <memory>
#include <mutex>
#include <vector>
#include <string>
namespace circuit {
  struct Branch {
    std::size_t firstNode;
    std::size_t secondNode;
  };
  
  struct ResistorBranch {
    Branch branch;
    double resistance;
  };
  
  struct CapacitorBranch {
    Branch branch;
    double capacitance;
    std::size_t stateIndex;
  };
  
  struct InductorBranch {
    Branch branch;
    double inductance;
    std::size_t stateIndex;
  };
  
  struct SourceBranch {
    Branch branch;
    std::unique_ptr<Genfun::AbsFunction> waveform;
  };
  
  struct DerivativeCache {
    std::mutex mutex;
    Genfun::Argument state;
    std::vector<double> derivatives;
    bool valid{};
  };
  
  struct CircuitSystem {
    std::size_t nodeCount{};
    std::size_t groundNode{};
    std::vector<ResistorBranch> resistors;
    std::vector<CapacitorBranch> capacitors;
    std::vector<InductorBranch> inductors;
    std::vector<SourceBranch> sources;
    std::vector<std::string> stateNames;
    std::shared_ptr<DerivativeCache> cache{std::make_shared<DerivativeCache>()};
  };
}

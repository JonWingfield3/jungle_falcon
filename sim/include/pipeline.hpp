#pragma once

#include <deque>
#include <memory>
#include <string>

#include <instruction_factory.hpp>
#include <instructions.hpp>
#include <memory.hpp>
#include <register_file.hpp>

class Pipeline;
using PipelinePtr = std::shared_ptr<Pipeline>;

class Pipeline {
 public:
  explicit Pipeline(RegFilePtr reg_file, PcPtr pc, MemoryPtr mem)
      : mem_(mem), pc_(pc), instruction_factory_(reg_file, pc, mem) {}

  enum class Stages { Fetch, Decode, Execute, MemoryAccess, WriteBack };

  int Fetch();
  void ExecuteCycle();

  std::vector<std::string> Instruction() const {
    std::vector<std::string> instruction_names_;
    for (const auto& instruction : instruction_queue_) {
      instruction_names_.push_back(instruction->Instruction());
    }
  }

  std::string Instruction(Stages stage) const {
    return instruction_queue_.at(static_cast<int>(stage))->Instruction();
  }

  void Reset() { instruction_queue_.clear(); }

 private:
  using InstructionPtr = std::shared_ptr<InstructionInterface>;
  using InstructionQueue = std::deque<InstructionPtr>;

  InstructionQueue instruction_queue_;
  InstructionFactory instruction_factory_;
  MemoryPtr mem_;
  PcPtr pc_;
};

#pragma once

#include <deque>
#include <memory>
#include <string>

#include <instruction_factory.hpp>
#include <instructions.hpp>
#include <memory.hpp>
#include <register_file.hpp>

class Pipeline {
 public:
  explicit Pipeline(RegisterFile& reg_file, ProgramCounter& pc, MemoryPtr mem)
      : mem_(mem), pc_(pc), instruction_factory_(reg_file, pc, mem) {}

  void Fetch() {
    const instr_t instr = mem_->Read<instr_t>(pc_.pc());
    InstructionPtr instr_ptr = instruction_factory_.Create(instr);

    if (!instruction_queue_.empty()) {
      instruction_queue_.pop_back();
    }
    instruction_queue_.push_front(instr_ptr);
  }

  void Run() {
#if defined(__INSTRUCTION_ACCURATE__)
    InstructionPtr instr_ptr = instruction_queue_.front();
    instr_ptr->Run();
#elif defined(__CYCLE_ACCURATE__)
#endif
  }

 private:
  using InstructionPtr = std::shared_ptr<InstructionInterface>;
  using InstructionQueue = std::deque<InstructionPtr>;
  InstructionQueue instruction_queue_;
  InstructionFactory instruction_factory_;
  MemoryPtr mem_;
  ProgramCounter& pc_;
};

#pragma once

#include <memory>

#include <instructions.hpp>
#include <register_file.hpp>
#include <riscv_defs.hpp>

class InstructionFactory {
 public:
  using InstructionPtr = std::shared_ptr<InstructionInterface>;
  InstructionFactory(RegisterFile& reg_file, ProgramCounter& pc,
                     MainMemory& mem)
      : reg_file_(reg_file), pc_(pc), mem_(mem) {}

  InstructionPtr Create(instr_t instr);

 private:
  MainMemory& mem_;
  RegisterFile& reg_file_;
  ProgramCounter& pc_;
};

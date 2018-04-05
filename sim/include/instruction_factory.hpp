#pragma once

#include <memory>

#include <instructions.hpp>
#include <memory.hpp>
#include <register_file.hpp>
#include <riscv_defs.hpp>

class InstructionFactory {
 public:
  InstructionFactory(RegFilePtr reg_file, PcPtr pc, MemoryPtr mem);

  InstructionPtr Create(instr_t instr);

 private:
  RegFilePtr reg_file_;
  PcPtr pc_;
  MemoryPtr mem_;
};

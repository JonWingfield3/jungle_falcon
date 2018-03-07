#pragma once

#include <instructions.hpp>
#include <register_file.hpp>
#include <riscv_defs.hpp>

class UTypeInstructionInterface : public InstructionInterface {
 public:
  explicit UTypeInstructionInterface(instr_t instr, RegisterFile& reg_file,
                                     ProgramCounter& pc)
      : InstructionInterface(instr), reg_file_(reg_file), pc_(pc) {
    name_ = "U Type Instruction";
  }

  union PACKED UTypeInstructionFormat {
    struct PACKED {
      instr_t opcode : 7;
      instr_t rd : 5;
      instr_t imm31_12 : 20;
    };
    instr_t word;
  };
  static_assert(sizeof(UTypeInstructionFormat) == 4,
                "U Type Instruction size != 4");

  void Decode() {
    UTypeInstructionFormat u_type_format;
    u_type_format.word = instr_;

    Rd_.first = static_cast<RegisterFile::Register>(u_type_format.rd);
    reg_file_.Read(Rd_);

    imm_ = static_cast<imm_t>(u_type_format.imm31_12 << 12);
  }

 protected:
  RegisterFile& reg_file_;
  ProgramCounter& pc_;
  RegisterFile::RegDataPair Rd_;
  imm_t imm_;
};

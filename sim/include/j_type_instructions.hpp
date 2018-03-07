#pragma once

#include <instructions.hpp>
#include <register_file.hpp>
#include <riscv_defs.hpp>

class JTypeInstructionInterface : public InstructionInterface {
 public:
  explicit JTypeInstructionInterface(instr_t instr, RegisterFile& reg_file,
                                     ProgramCounter& pc)
      : InstructionInterface(instr), reg_file_(reg_file), pc_(pc) {
    name_ = "J Type Instruction";
  }

  union PACKED JTypeInstructionFormat {
    struct PACKED {
      instr_t opcode : 7;
      instr_t rd : 5;
      instr_t imm19_12 : 8;
      instr_t imm11 : 1;
      instr_t imm10_1 : 10;
      instr_t imm20 : 1;
    };
    instr_t word;
  };
  static_assert(sizeof(JTypeInstructionFormat) == 4,
                "J Type Instruction size != 4");

  void Decode() {
    JTypeInstructionFormat j_type_format;
    j_type_format.word = instr_;

    Rd_.first = static_cast<RegisterFile::Register>(j_type_format.rd);
    Rd_.second = reg_file_.Read(Rd_.first);
    VLOG(3) << name_ << ": rd = " << j_type_format.rd << ", " << Rd_.second;

    imm_ = static_cast<imm_t>(
        (j_type_format.imm20 << 20) | (j_type_format.imm19_12 << 12) |
        (j_type_format.imm11 << 11) | (j_type_format.imm10_1 << 1));
    VLOG(3) << name_ << ": imm = " << imm_;
  }

  RegisterMask Dependencies() { return dependencies_; }

 protected:
  RegisterFile& reg_file_;
  ProgramCounter& pc_;
  RegDataPair Rd_;
  imm_t imm_;
};

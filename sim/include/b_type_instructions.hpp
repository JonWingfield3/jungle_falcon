#pragma once

#include <instructions.hpp>
#include <register_file.hpp>
#include <riscv_defs.hpp>

class BTypeInstructionInterface : public InstructionInterface {
 public:
  explicit BTypeInstructionInterface(instr_t instr, RegisterFile& reg_file,
                                     ProgramCounter& pc)
      : InstructionInterface(instr), reg_file_(reg_file), pc_(pc) {
    name_ = "B Type Instruction";
  }

  union PACKED BTypeInstructionFormat {
    struct PACKED {
      instr_t opcode : 7;
      instr_t imm11 : 1;
      instr_t imm4_1 : 4;
      instr_t funct3 : 3;
      instr_t rs1 : 5;
      instr_t rs2 : 5;
      instr_t imm10_5 : 6;
      instr_t imm12 : 1;
    };
    instr_t word;
  };
  static_assert(sizeof(BTypeInstructionFormat) == 4,
                "B Type Instruction size != 4");

  void Decode() {
    BTypeInstructionFormat b_type_format;
    b_type_format.word = instr_;

    Rs1_.first = static_cast<RegisterFile::Register>(b_type_format.rs1);
    Rs1_.second = reg_file_.Read(Rs1_.first);
    VLOG(3) << name_ << ": rs1 = " << b_type_format.rs1 << ", " << Rs1_.second;

    Rs2_.first = static_cast<RegisterFile::Register>(b_type_format.rs2);
    Rs2_.second = reg_file_.Read(Rs2_.first);
    VLOG(3) << name_ << ": rd = " << b_type_format.rs2 << ", " << Rs2_.second;

    imm_ = static_cast<imm_t>((b_type_format.imm12 << 12) |
                              (b_type_format.imm10_5 << 5) |
                              (b_type_format.imm4_1 << 1));
    VLOG(3) << name_ << ": imm = " << imm_;

    dependencies_.set(b_type_format.rs1);
    dependencies_.set(b_type_format.rs2);
  }

  RegisterMask Dependencies() { return dependencies_; }

 protected:
  RegisterFile& reg_file_;
  ProgramCounter& pc_;
  RegDataPair Rs1_;
  RegDataPair Rs2_;
  imm_t imm_;
};

#pragma once

#include <instructions.hpp>
#include <register_file.hpp>
#include <riscv_defs.hpp>

class JalInstruction : public InstructionInterface {
 public:
  explicit JalInstruction(instr_t instr, RegisterFile& reg_file,
                          ProgramCounter& pc)
      : InstructionInterface(instr), reg_file_(reg_file), pc_(pc) {
    name_ = "Jal Instruction";
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
    reg_file_.Read(Rd_);

    imm_ = static_cast<imm_t>(
        (j_type_format.imm20 << 20) | (j_type_format.imm19_12 << 12) |
        (j_type_format.imm11 << 11) | (j_type_format.imm10_1 << 1));

    VLOG(2) << name_ << " "
            << "Rd: " << j_type_format.rd << ", Imm: " << imm_;
  }

  void Execute() final {
    Rd_.second = pc_.pc() + 4;
    VLOG(4) << "Jump address to write to Rd = " << Rd_.second;
  }

  void MemoryAccess() final {}

  void WriteBack() final {
    pc_ += imm_;
    reg_file_.Write(Rd_);
  }

 protected:
  RegisterFile& reg_file_;
  ProgramCounter& pc_;
  mem_addr_t jump_addr_;
  RegisterFile::RegDataPair Rd_;
  imm_t imm_;
};

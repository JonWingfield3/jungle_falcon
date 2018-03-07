#pragma once

#include <instructions.hpp>
#include <register_file.hpp>
#include <riscv_defs.hpp>

class ITypeInstructionInterface : public InstructionInterface {
 public:
  explicit ITypeInstructionInterface(instr_t instr, RegisterFile& reg_file)
      : InstructionInterface(instr), reg_file_(reg_file) {
    name_ = "I Type Instruction";
  }

  union PACKED ITypeInstructionFormat {
    struct PACKED {
      instr_t opcode : 7;
      instr_t rd : 5;
      instr_t funct3 : 3;
      instr_t rs1 : 5;
      instr_t imm11_0 : 12;
    };
    instr_t word;
  };
  static_assert(sizeof(ITypeInstructionFormat) == 4,
                "I Type Instruction size != 4");

  void Decode() final {
    ITypeInstructionFormat i_type_format;
    i_type_format.word = instr_;

    Rd_.first = static_cast<RegisterFile::Register>(i_type_format.rd);
    reg_file_.Read(Rd_);

    Rs1_.first = static_cast<RegisterFile::Register>(i_type_format.rs1);
    reg_file_.Read(Rs1_);

    imm_ = static_cast<imm_t>(i_type_format.imm11_0);

    dependencies_.set(i_type_format.rd);
    dependencies_.set(i_type_format.rs1);

    VLOG(2) << name_ << " "
            << "Rd: " << i_type_format.rd << ", Rs1: " << i_type_format.rs1
            << ", Imm: " << imm_;
  }

  void MemoryAccess() final {
    VLOG(5) << name_ << " I Type Memory Access stage doing nothing";
  }

  void WriteBack() final {
    VLOG(5) << name_ << " Write Back stage: writing " << Rd_.second
            << " to Register " << Rd_.second;
    reg_file_.Write(Rd_);
  }

 protected:
  RegisterFile& reg_file_;
  RegisterFile::RegDataPair Rd_;
  RegisterFile::RegDataPair Rs1_;
  imm_t imm_;
};

class AddiInstruction : public ITypeInstructionInterface {
 public:
  AddiInstruction(instr_t instr, RegisterFile& reg_file)
      : ITypeInstructionInterface(instr, reg_file) {
    name_ = "Addi";
  }

  void Execute() final { Rd_.second = Rs1_.second + imm_; }
};

class SltiInstruction : public ITypeInstructionInterface {
 public:
  SltiInstruction(instr_t instr, RegisterFile& reg_file)
      : ITypeInstructionInterface(instr, reg_file) {
    name_ = "Slti";
  }

  void Execute() final {
    Rd_.second = static_cast<signed_reg_data_t>(Rs1_.second) < imm_;
  }
};

class SltiuInstruction : public ITypeInstructionInterface {
 public:
  SltiuInstruction(instr_t instr, RegisterFile& reg_file)
      : ITypeInstructionInterface(instr, reg_file) {
    name_ = "Sltiu";
  }

  void Execute() final { Rd_.second = Rs1_.second < imm_; }
};

class XoriInstruction : public ITypeInstructionInterface {
 public:
  XoriInstruction(instr_t instr, RegisterFile& reg_file)
      : ITypeInstructionInterface(instr, reg_file) {
    name_ = "Xori";
  }

  void Execute() final { Rd_.second = Rs1_.second ^ imm_; }
};

class OriInstruction : public ITypeInstructionInterface {
 public:
  OriInstruction(instr_t instr, RegisterFile& reg_file)
      : ITypeInstructionInterface(instr, reg_file) {
    name_ = "Ori";
  }

  void Execute() final { Rd_.second = Rs1_.second | imm_; }
};

class AndiInstruction : public ITypeInstructionInterface {
 public:
  AndiInstruction(instr_t instr, RegisterFile& reg_file)
      : ITypeInstructionInterface(instr, reg_file) {
    name_ = "Andi";
  }

  void Execute() final { Rd_.second = Rs1_.second & imm_; }
};

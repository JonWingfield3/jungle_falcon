#pragma once

#include <instructions.hpp>
#include <register_file.hpp>
#include <riscv_defs.hpp>

class RTypeInstructionInterface : public InstructionInterface {
 public:
  explicit RTypeInstructionInterface(instr_t instr, RegisterFile& reg_file)
      : InstructionInterface(instr), reg_file_(reg_file) {
    name_ = "R Type Instruction";
  }

  union PACKED RTypeInstructionFormat {
    struct PACKED {
      instr_t opcode : 7;
      instr_t rd : 5;
      instr_t funct3 : 3;
      instr_t rs1 : 5;
      instr_t rs2 : 5;
      instr_t funct7 : 7;
    };
    instr_t word;
  };
  static_assert(sizeof(RTypeInstructionFormat) == 4,
                "R Type Instruction size != 4");

  void Decode() final {
    RTypeInstructionFormat r_type_format;
    r_type_format.word = instr_;

    Rd_.first = static_cast<RegisterFile::Register>(r_type_format.rd);
    reg_file_.Read(Rd_);

    Rs1_.first = static_cast<RegisterFile::Register>(r_type_format.rs1);
    reg_file_.Read(Rs1_);

    Rs2_.first = static_cast<RegisterFile::Register>(r_type_format.rs2);
    reg_file_.Read(Rs2_);

    dependencies_.set(r_type_format.rd);
    dependencies_.set(r_type_format.rs1);
    dependencies_.set(r_type_format.rs2);

    VLOG(2) << name_ << " "
            << "Rd: " << r_type_format.rd << ", Rs1: " << r_type_format.rs1
            << ", Rs2: " << r_type_format.rs2;
  }

  void MemoryAccess() final {
    VLOG(5) << name_ << " R Type Memory Access stage doing nothing";
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
  RegisterFile::RegDataPair Rs2_;
};

class SlliInstruction : public RTypeInstructionInterface {
 public:
  SlliInstruction(instr_t instr, RegisterFile& reg_file)
      : RTypeInstructionInterface(instr, reg_file) {
    name_ = "Slli";
  }

  void Execute() final {
    const unsigned shamt = static_cast<unsigned>(Rs2_.first);
    Rd_.second = Rs1_.second << shamt;
  }
};

class SrliInstruction : public RTypeInstructionInterface {
 public:
  SrliInstruction(instr_t instr, RegisterFile& reg_file)
      : RTypeInstructionInterface(instr, reg_file) {
    name_ = "Srli";
  }

  void Execute() final {
    const unsigned shamt = static_cast<unsigned>(Rs2_.first);
    Rd_.second = Rs1_.second >> shamt;
  }
};

class SraiInstruction : public RTypeInstructionInterface {
 public:
  SraiInstruction(instr_t instr, RegisterFile& reg_file)
      : RTypeInstructionInterface(instr, reg_file) {
    name_ = "Srai";
  }

  void Execute() final {
    const unsigned shamt = static_cast<unsigned>(Rs2_.first);
    Rd_.second = static_cast<signed_reg_data_t>(Rs1_.second) >> shamt;
  }
};

class AddInstruction : public RTypeInstructionInterface {
 public:
  AddInstruction(instr_t instr, RegisterFile& reg_file)
      : RTypeInstructionInterface(instr, reg_file) {
    name_ = "Add";
  }

  void Execute() final { Rd_.second = Rs1_.second + Rs2_.second; }
};

class SubInstruction : public RTypeInstructionInterface {
 public:
  SubInstruction(instr_t instr, RegisterFile& reg_file)
      : RTypeInstructionInterface(instr, reg_file) {
    name_ = "Sub";
  }

  void Execute() final { Rd_.second = Rs1_.second - Rs2_.second; }
};

class SllInstruction : public RTypeInstructionInterface {
 public:
  SllInstruction(instr_t instr, RegisterFile& reg_file)
      : RTypeInstructionInterface(instr, reg_file) {
    name_ = "Sll";
  }

  void Execute() final { Rd_.second = Rs1_.second << Rs2_.second; }
};

class SltInstruction : public RTypeInstructionInterface {
 public:
  SltInstruction(instr_t instr, RegisterFile& reg_file)
      : RTypeInstructionInterface(instr, reg_file) {
    name_ = "Slt";
  }

  void Execute() final {
    Rd_.second = static_cast<signed_reg_data_t>(Rs1_.second) <
                 static_cast<signed_reg_data_t>(Rs2_.second);
  }
};

class SltuInstruction : public RTypeInstructionInterface {
 public:
  SltuInstruction(instr_t instr, RegisterFile& reg_file)
      : RTypeInstructionInterface(instr, reg_file) {
    name_ = "Sltu";
  }

  void Execute() final { Rd_.second = Rs1_.second < Rs2_.second; }
};

class XorInstruction : public RTypeInstructionInterface {
 public:
  XorInstruction(instr_t instr, RegisterFile& reg_file)
      : RTypeInstructionInterface(instr, reg_file) {
    name_ = "Xor";
  }

  void Execute() final { Rd_.second = Rs1_.second ^ Rs2_.second; }
};

class SrlInstruction : public RTypeInstructionInterface {
 public:
  SrlInstruction(instr_t instr, RegisterFile& reg_file)
      : RTypeInstructionInterface(instr, reg_file) {
    name_ = "Srl";
  }

  void Execute() final { Rd_.second = Rs1_.second >> Rs2_.second; }
};

class SraInstruction : public RTypeInstructionInterface {
 public:
  SraInstruction(instr_t instr, RegisterFile& reg_file)
      : RTypeInstructionInterface(instr, reg_file) {
    name_ = "Sra";
  }

  void Execute() final {
    Rd_.second = static_cast<signed_reg_data_t>(Rs1_.second) >> Rs2_.second;
  }
};

class OrInstruction : public RTypeInstructionInterface {
 public:
  OrInstruction(instr_t instr, RegisterFile& reg_file)
      : RTypeInstructionInterface(instr, reg_file) {
    name_ = "Or";
  }

  void Execute() final { Rd_.second = Rs1_.second | Rs2_.second; }
};

class AndInstruction : public RTypeInstructionInterface {
 public:
  AndInstruction(instr_t instr, RegisterFile& reg_file)
      : RTypeInstructionInterface(instr, reg_file) {
    name_ = "And";
  }

  void Execute() final { Rd_.second = Rs1_.second & Rs2_.second; }
};

#pragma once

#include <memory>

#include <instructions.hpp>
#include <register_file.hpp>
#include <riscv_defs.hpp>

class RTypeInstructionInterface : public InstructionInterface {
 public:
  explicit RTypeInstructionInterface(instr_t instr, RegFilePtr reg_file)
      : InstructionInterface(instr), reg_file_(reg_file) {
    name_ = "R Type Instruction";
  }

  ~RTypeInstructionInterface() override = default;

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
    reg_file_->Read(Rd_);

    Rs1_.first = static_cast<RegisterFile::Register>(r_type_format.rs1);
    reg_file_->Read(Rs1_);

    Rs2_.first = static_cast<RegisterFile::Register>(r_type_format.rs2);
    reg_file_->Read(Rs2_);

    dependencies_.set(r_type_format.rd);
    dependencies_.set(r_type_format.rs1);
    dependencies_.set(r_type_format.rs2);

    InstructionInterface::Decode();
  }

  void Execute() {
    VLOG(3) << "Executed: " << Rd_ << " = " << name_ << "{" << Rs1_ << ", "
            << Rs2_ << "}";
  }

  void MemoryAccess() final {
    VLOG(3) << name_ << " MemoryAccess stage doing nothing";
  }

  void WriteBack() final {
    VLOG(3) << name_ << " WriteBack stage: Writing " << Rd_
            << " back to RegFile";
    reg_file_->Write(Rd_);
  }

 protected:
  void SetInstructionName() {
    std::stringstream instruction_stream;
    instruction_stream << name_ << " x" << static_cast<int>(Rd_.first) << ", x"
                       << static_cast<int>(Rs1_.first) << ", x"
                       << static_cast<int>(Rs2_.first);
    instruction_ = instruction_stream.str();
  }

  RegFilePtr reg_file_;
  RegisterFile::RegDataPair Rd_;
  RegisterFile::RegDataPair Rs1_;
  RegisterFile::RegDataPair Rs2_;
};

class SxxiInstructionInterface : public RTypeInstructionInterface {
 public:
  SxxiInstructionInterface(instr_t instr, RegFilePtr reg_file)
      : RTypeInstructionInterface(instr, reg_file) {
    name_ = "slli";
  }

  void Execute() {
    shamt_ = static_cast<unsigned>(Rs2_.first);
    VLOG(3) << "Executed: " << Rd_ << " = " << name_ << "{" << Rs1_ << ", "
            << Rs2_.second << "}";
  }

 protected:
  void SetInstructionName() final {
    std::stringstream instruction_stream;
    instruction_stream << name_ << " "
                       << "x" << static_cast<int>(Rd_.first) << ", x"
                       << static_cast<int>(Rs1_.first) << ", "
                       << static_cast<int>(Rs2_.first);
    instruction_ = instruction_stream.str();
  }
  int shamt_;
};

class SlliInstruction : public SxxiInstructionInterface {
 public:
  SlliInstruction(instr_t instr, RegFilePtr reg_file)
      : SxxiInstructionInterface(instr, reg_file) {
    name_ = "slli";
  }

  void Execute() final {
    SxxiInstructionInterface::Execute();
    Rd_.second = Rs1_.second << shamt_;
  }
};

class SrliInstruction : public SxxiInstructionInterface {
 public:
  SrliInstruction(instr_t instr, RegFilePtr reg_file)
      : SxxiInstructionInterface(instr, reg_file) {
    name_ = "srli";
  }

  void Execute() final {
    SxxiInstructionInterface::Execute();
    Rd_.second = Rs1_.second >> shamt_;
  }
};

class SraiInstruction : public SxxiInstructionInterface {
 public:
  SraiInstruction(instr_t instr, RegFilePtr reg_file)
      : SxxiInstructionInterface(instr, reg_file) {
    name_ = "srai";
  }

  void Execute() final {
    SxxiInstructionInterface::Execute();
    const int sign_carry_mask = (Rs1_.second & (1 << 31)) ? -1 : 0;
    const int sign_ext_mask = (sign_carry_mask & ((1 << shamt_) - 1))
                              << ((8 * sizeof(reg_data_t)) - shamt_);
    Rd_.second = Rs1_.second >> shamt_;
    Rd_.second |= sign_ext_mask;
  }
};

class AddInstruction : public RTypeInstructionInterface {
 public:
  AddInstruction(instr_t instr, RegFilePtr reg_file)
      : RTypeInstructionInterface(instr, reg_file) {
    name_ = "add";
  }

  void Execute() final {
    Rd_.second = Rs1_.second + Rs2_.second;
    RTypeInstructionInterface::Execute();
  }
};

class SubInstruction : public RTypeInstructionInterface {
 public:
  SubInstruction(instr_t instr, RegFilePtr reg_file)
      : RTypeInstructionInterface(instr, reg_file) {
    name_ = "sub";
  }

  void Execute() final {
    Rd_.second = Rs1_.second - Rs2_.second;
    RTypeInstructionInterface::Execute();
  }
};

class SllInstruction : public RTypeInstructionInterface {
 public:
  SllInstruction(instr_t instr, RegFilePtr reg_file)
      : RTypeInstructionInterface(instr, reg_file) {
    name_ = "sll";
  }

  void Execute() final {
    Rd_.second = Rs1_.second << Rs2_.second;
    RTypeInstructionInterface::Execute();
  }
};

class SltInstruction : public RTypeInstructionInterface {
 public:
  SltInstruction(instr_t instr, RegFilePtr reg_file)
      : RTypeInstructionInterface(instr, reg_file) {
    name_ = "slt";
  }

  void Execute() final {
    Rd_.second = static_cast<signed_reg_data_t>(Rs1_.second) <
                 static_cast<signed_reg_data_t>(Rs2_.second);
    RTypeInstructionInterface::Execute();
  }
};

class SltuInstruction : public RTypeInstructionInterface {
 public:
  SltuInstruction(instr_t instr, RegFilePtr reg_file)
      : RTypeInstructionInterface(instr, reg_file) {
    name_ = "sltu";
  }

  void Execute() final {
    Rd_.second = Rs1_.second < Rs2_.second;
    RTypeInstructionInterface::Execute();
  }
};

class XorInstruction : public RTypeInstructionInterface {
 public:
  XorInstruction(instr_t instr, RegFilePtr reg_file)
      : RTypeInstructionInterface(instr, reg_file) {
    name_ = "xor";
  }

  void Execute() final {
    Rd_.second = Rs1_.second ^ Rs2_.second;
    RTypeInstructionInterface::Execute();
  }
};

class SrlInstruction : public RTypeInstructionInterface {
 public:
  SrlInstruction(instr_t instr, RegFilePtr reg_file)
      : RTypeInstructionInterface(instr, reg_file) {
    name_ = "srl";
  }

  void Execute() final {
    Rd_.second = Rs1_.second >> Rs2_.second;
    RTypeInstructionInterface::Execute();
  }
};

class SraInstruction : public RTypeInstructionInterface {
 public:
  SraInstruction(instr_t instr, RegFilePtr reg_file)
      : RTypeInstructionInterface(instr, reg_file) {
    name_ = "sra";
  }

  void Execute() final {
    const int sign_carry_mask = (Rs1_.second & (1 << 31)) ? -1 : 0;
    const int sign_ext_mask = (sign_carry_mask & ((1 << Rs2_.second) - 1));
    Rd_.second = Rs1_.second >> Rs2_.second;
    Rd_.second |= sign_ext_mask;
    RTypeInstructionInterface::Execute();
  }
};

class OrInstruction : public RTypeInstructionInterface {
 public:
  OrInstruction(instr_t instr, RegFilePtr reg_file)
      : RTypeInstructionInterface(instr, reg_file) {
    name_ = "or";
  }

  void Execute() final {
    Rd_.second = Rs1_.second | Rs2_.second;
    RTypeInstructionInterface::Execute();
  }
};

class AndInstruction : public RTypeInstructionInterface {
 public:
  AndInstruction(instr_t instr, RegFilePtr reg_file)
      : RTypeInstructionInterface(instr, reg_file) {
    name_ = "and";
  }

  void Execute() final {
    Rd_.second = Rs1_.second & Rs2_.second;
    RTypeInstructionInterface::Execute();
  }
};

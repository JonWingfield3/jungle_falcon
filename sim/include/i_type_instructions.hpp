#pragma once

#include <memory>

#include <instructions.hpp>
#include <register_file.hpp>
#include <riscv_defs.hpp>

class ITypeInstructionInterface : public InstructionInterface {
 public:
  explicit ITypeInstructionInterface(instr_t instr, RegFilePtr reg_file)
      : InstructionInterface(instr), reg_file_(reg_file) {
    name_ = "I Type Instruction";
  }

  ~ITypeInstructionInterface() override = default;

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
    reg_file_->Read(Rd_);

    Rs1_.first = static_cast<RegisterFile::Register>(i_type_format.rs1);
    reg_file_->Read(Rs1_);

    imm_t imm_upper_20 = ((i_type_format.imm11_0 & (1 << 11)) ? -1 : 0);
    imm_upper_20 &= ~(0xfff);
    imm_ = static_cast<imm_t>(imm_upper_20 | i_type_format.imm11_0);

    dependencies_.set(i_type_format.rd);
    dependencies_.set(i_type_format.rs1);

    InstructionInterface::Decode();
  }

  void Execute() {
    VLOG(3) << "Executed: " << Rd_ << " = " << name_ << "{" << Rs1_ << ", "
            << imm_ << "}";
  }

  void WriteBack() {
    VLOG(3) << name_ << " WriteBack stage: writing " << Rd_
            << " back to regfile";
    reg_file_->Write(Rd_);
  }

 protected:
  void SetInstructionName() {
    std::stringstream instruction_stream;
    instruction_stream << name_ << " x" << static_cast<int>(Rd_.first) << ", x"
                       << static_cast<int>(Rs1_.first) << ", " << imm_;
    instruction_ = instruction_stream.str();
  }

  RegFilePtr reg_file_;
  RegisterFile::RegDataPair Rd_;
  RegisterFile::RegDataPair Rs1_;
  imm_t imm_;
};

class AddiInstruction : public ITypeInstructionInterface {
 public:
  AddiInstruction(instr_t instr, RegFilePtr reg_file)
      : ITypeInstructionInterface(instr, reg_file) {
    name_ = "addi";
  }

  void Execute() final {
    Rd_.second = Rs1_.second + imm_;
    ITypeInstructionInterface::Execute();
  }
};

class SltiInstruction : public ITypeInstructionInterface {
 public:
  SltiInstruction(instr_t instr, RegFilePtr reg_file)
      : ITypeInstructionInterface(instr, reg_file) {
    name_ = "slti";
  }

  void Execute() final {
    Rd_.second = static_cast<signed_reg_data_t>(Rs1_.second) < imm_;
    ITypeInstructionInterface::Execute();
  }
};

class SltiuInstruction : public ITypeInstructionInterface {
 public:
  SltiuInstruction(instr_t instr, RegFilePtr reg_file)
      : ITypeInstructionInterface(instr, reg_file) {
    name_ = "sltiu";
  }

  void Execute() final {
    Rd_.second = Rs1_.second < imm_;
    ITypeInstructionInterface::Execute();
  }
};

class XoriInstruction : public ITypeInstructionInterface {
 public:
  XoriInstruction(instr_t instr, RegFilePtr reg_file)
      : ITypeInstructionInterface(instr, reg_file) {
    name_ = "xori";
  }

  void Execute() final {
    Rd_.second = Rs1_.second ^ imm_;
    ITypeInstructionInterface::Execute();
  }
};

class OriInstruction : public ITypeInstructionInterface {
 public:
  OriInstruction(instr_t instr, RegFilePtr reg_file)
      : ITypeInstructionInterface(instr, reg_file) {
    name_ = "ori";
  }

  void Execute() final {
    Rd_.second = Rs1_.second | imm_;
    ITypeInstructionInterface::Execute();
  }
};

class AndiInstruction : public ITypeInstructionInterface {
 public:
  AndiInstruction(instr_t instr, RegFilePtr reg_file)
      : ITypeInstructionInterface(instr, reg_file) {
    name_ = "andi";
  }

  void Execute() final {
    Rd_.second = Rs1_.second & imm_;
    ITypeInstructionInterface::Execute();
  }
};

class JalrInstruction : public ITypeInstructionInterface {
 public:
  JalrInstruction(instr_t instr, RegFilePtr reg_file, PcPtr pc)
      : ITypeInstructionInterface(instr, reg_file), pc_(pc) {}

  void Execute() final {
    target_addr_ = Rs1_.second + imm_;
    target_addr_ &= ~static_cast<mem_addr_t>(1);
    VLOG(3) << "Execute: Target address = " << target_addr_;
  }

  void WriteBack() final {
    pc_->Jump(target_addr_);
    VLOG(3) << "WriteBack: Jumping to " << target_addr_;
  }

 private:
  PcPtr pc_;
  mem_addr_t target_addr_;
};

class LoadInstructionInterface : public ITypeInstructionInterface {
 public:
  LoadInstructionInterface(instr_t instr, RegFilePtr reg_file, MemoryPtr mem)
      : ITypeInstructionInterface(instr, reg_file), mem_(mem) {
    name_ = "load";
  }

  void Execute() final {
    load_addr_ = Rs1_.second + imm_;
    VLOG(3) << "Execute: calculated load address as " << load_addr_;
  }

  void MemoryAccess() {
    VLOG(3) << "MemoryAccess: read " << load_data_ << " from " << load_addr_;
  }

  void WriteBack() final {
    Rd_.second = load_data_;
    reg_file_->Write(Rd_);
    VLOG(3) << "WriteBack: writing " << load_data_ << " to " << Rd_;
  }

 protected:
  void SetInstructionName() final {
    std::stringstream instruction_stream;
    instruction_stream << name_ << " x" << static_cast<int>(Rd_.first) << ", "
                       << imm_ << "(x" << static_cast<int>(Rs1_.first) << ")";
    instruction_ = instruction_stream.str();
  }

  MemoryPtr mem_;
  mem_addr_t load_addr_;
  reg_data_t load_data_;
};

class LbInstruction : public LoadInstructionInterface {
 public:
  LbInstruction(instr_t instr, RegFilePtr reg_file, MemoryPtr mem)
      : LoadInstructionInterface(instr, reg_file, mem) {
    name_ = "lb";
  }

  void MemoryAccess() final {
    load_data_ = static_cast<signed_reg_data_t>(mem_->ReadByte(load_addr_));
    LoadInstructionInterface::MemoryAccess();
  }
};

class LbuInstruction : public LoadInstructionInterface {
 public:
  LbuInstruction(instr_t instr, RegFilePtr reg_file, MemoryPtr mem)
      : LoadInstructionInterface(instr, reg_file, mem) {
    name_ = "lbu";
  }

  void MemoryAccess() final {
    load_data_ = static_cast<reg_data_t>(mem_->ReadByte(load_addr_));
    LoadInstructionInterface::MemoryAccess();
  }
};

class LhInstruction : public LoadInstructionInterface {
 public:
  LhInstruction(instr_t instr, RegFilePtr reg_file, MemoryPtr mem)
      : LoadInstructionInterface(instr, reg_file, mem) {
    name_ = "lh";
  }

  void MemoryAccess() final {
    load_data_ = static_cast<signed_reg_data_t>(mem_->ReadHalfWord(load_addr_));
    LoadInstructionInterface::MemoryAccess();
  }
};

class LhuInstruction : public LoadInstructionInterface {
 public:
  LhuInstruction(instr_t instr, RegFilePtr reg_file, MemoryPtr mem)
      : LoadInstructionInterface(instr, reg_file, mem) {
    name_ = "lhu";
  }

  void MemoryAccess() final {
    load_data_ = static_cast<reg_data_t>(mem_->ReadHalfWord(load_addr_));
    LoadInstructionInterface::MemoryAccess();
  }
};

class LwInstruction : public LoadInstructionInterface {
 public:
  LwInstruction(instr_t instr, RegFilePtr reg_file, MemoryPtr mem)
      : LoadInstructionInterface(instr, reg_file, mem) {
    name_ = "lw";
  }

  void MemoryAccess() final {
    load_data_ = static_cast<reg_data_t>(mem_->ReadWord(load_addr_));
    LoadInstructionInterface::MemoryAccess();
  }
};

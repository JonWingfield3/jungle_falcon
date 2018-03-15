#pragma once

#include <memory>

#include <instructions.hpp>
#include <register_file.hpp>
#include <riscv_defs.hpp>

class UTypeInstructionInterface : public InstructionInterface {
 public:
  explicit UTypeInstructionInterface(instr_t instr, RegFilePtr reg_file)
      : InstructionInterface(instr), reg_file_(reg_file) {
    name_ = "U Type Instruction";
  }

  ~UTypeInstructionInterface() override = default;

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
    reg_file_->Read(Rd_);

    imm_ = static_cast<imm_t>(u_type_format.imm31_12 << 12);

    InstructionInterface::Decode();
  }

 protected:
  void SetInstructionName() {
    std::stringstream instruction_stream;
    instruction_stream << name_ << " x" << static_cast<int>(Rd_.first) << ", "
                       << imm_;
    instruction_ = instruction_stream.str();
  }

  RegFilePtr reg_file_;
  RegisterFile::RegDataPair Rd_;
  imm_t imm_;
};

class LuiInstruction : public UTypeInstructionInterface {
 public:
  LuiInstruction(instr_t instr, RegFilePtr reg_file)
      : UTypeInstructionInterface(instr, reg_file) {
    name_ = "lui";
  }

  void Execute() final { Rd_.second = imm_; }

  void MemoryAccess() final {}

  void WriteBack() final {
    reg_file_->Write(Rd_);
    VLOG(3) << "WriteBack: writing immediate to register file " << Rd_;
  }
};

class AuipcInstruction : public UTypeInstructionInterface {
 public:
  AuipcInstruction(instr_t instr, RegFilePtr reg_file, PcPtr pc)
      : UTypeInstructionInterface(instr, reg_file), pc_(pc) {
    name_ = "auipc";
  }

  void Execute() final {
    reg_data_t pc_offset = static_cast<reg_data_t>(
        static_cast<int>(pc_->Reg()) + static_cast<int>(imm_));
    VLOG(3) << "Execute: computed pc offset address " << pc_offset;
    Rd_.second = pc_offset;
  }

  void MemoryAccess() final {}

  void WriteBack() final {
    reg_file_->Write(Rd_);
    VLOG(3) << "WriteBack: writing to register file " << Rd_;
  }

 private:
  PcPtr pc_;
};

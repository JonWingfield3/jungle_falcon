#pragma once

#include <memory>

#include <instructions.hpp>
#include <register_file.hpp>
#include <riscv_defs.hpp>

class JalInstruction : public InstructionInterface {
 public:
  explicit JalInstruction(instr_t instr, RegFilePtr reg_file, PcPtr pc)
      : InstructionInterface(instr), reg_file_(reg_file), pc_(pc) {
    name_ = "jal";
  }

  ~JalInstruction() override = default;

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

  void Decode() final {
    JTypeInstructionFormat j_type_format;
    j_type_format.word = instr_;

    Rd_.first = static_cast<RegisterFile::Register>(j_type_format.rd);
    reg_file_->Read(Rd_);

    imm_t imm_upper_11 = (j_type_format.imm20 ? -1 : 0);
    imm_upper_11 &= ~(0xfffff);
    imm_ = static_cast<imm_t>(imm_upper_11 | (j_type_format.imm20 << 20) |
                              (j_type_format.imm19_12 << 12) |
                              (j_type_format.imm11 << 11) |
                              (j_type_format.imm10_1 << 1));

    InstructionInterface::Decode();
  }

  void Execute() final {
    Rd_.second = pc_->Reg() + sizeof(instr_t);
    VLOG(3) << "Execute: Computed return address as " << Rd_;
  }

  void WriteBack() final {
    const reg_data_t old_pc = pc_->Reg();
    pc_->Branch(imm_);
    reg_file_->Write(Rd_);
    VLOG(3) << "WriteBack: Changing pc from " << old_pc << " to " << pc_->Reg();
    VLOG(3) << "WriteBack: Writing " << Rd_ << " back to RegFile";
  }

 protected:
  void SetInstructionName() {
    std::stringstream instruction_stream;
    instruction_stream << name_ << " x" << static_cast<int>(Rd_.first) << ", "
                       << imm_;
    instruction_ = instruction_stream.str();
  }

  RegFilePtr reg_file_;
  PcPtr pc_;
  RegisterFile::RegDataPair Rd_;
  imm_t imm_;
};

#pragma once

#include <memory>

#include <instructions.hpp>
#include <register_file.hpp>
#include <riscv_defs.hpp>

// TODO: Add actual branching (which stage?)

class BTypeInstructionInterface : public InstructionInterface {
 public:
  explicit BTypeInstructionInterface(instr_t instr, RegFilePtr reg_file,
                                     PcPtr& pc)
      : InstructionInterface(instr), reg_file_(reg_file), pc_(pc) {
    name_ = "B Type Instruction";
  }

  ~BTypeInstructionInterface() override = default;

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
    reg_file_->Read(Rs1_);

    Rs2_.first = static_cast<RegisterFile::Register>(b_type_format.rs2);
    reg_file_->Read(Rs2_);

    imm_t imm_upper_20 = (b_type_format.imm12 ? -1 : 0);
    imm_upper_20 &= ~(0x1fff);
    imm_ = static_cast<int>(imm_upper_20 | (b_type_format.imm12 << 12) |
                            (b_type_format.imm11 << 11) |
                            (b_type_format.imm10_5 << 5) |
                            (b_type_format.imm4_1 << 1));

    dependencies_.set(b_type_format.rs1);
    dependencies_.set(b_type_format.rs2);

    InstructionInterface::Decode();
  }

  void Execute() {
    VLOG(3) << "Execute: " << name_
            << (branch_ ? " taking branch" : " not taking branch");
  }

  void WriteBack() final {
    if (branch_) {
      pc_->Branch(imm_);
    }
    VLOG(3) << "WriteBack: setting pc to " << pc_->Reg();
  }

 protected:
  void SetInstructionName() {
    std::stringstream instruction_stream;
    instruction_stream << name_ << " x" << static_cast<int>(Rs1_.first) << ", x"
                       << static_cast<int>(Rs2_.first) << ", " << imm_;
    instruction_ = instruction_stream.str();
  }

  RegFilePtr reg_file_;
  PcPtr pc_;
  mem_addr_t target_addr_;
  bool branch_;
  RegisterFile::RegDataPair Rs1_;
  RegisterFile::RegDataPair Rs2_;
  int imm_;
};

class BeqInstruction : public BTypeInstructionInterface {
 public:
  BeqInstruction(instr_t instr, RegFilePtr reg_file, PcPtr pc)
      : BTypeInstructionInterface(instr, reg_file, pc) {
    name_ = "beq";
  }

  void Execute() final {
    branch_ = (Rs1_.second == Rs2_.second);
    BTypeInstructionInterface::Execute();
  }
};

class BneInstruction : public BTypeInstructionInterface {
 public:
  BneInstruction(instr_t instr, RegFilePtr reg_file, PcPtr pc)
      : BTypeInstructionInterface(instr, reg_file, pc) {
    name_ = "bne";
  }

  void Execute() final {
    branch_ = (Rs1_.second != Rs2_.second);
    BTypeInstructionInterface::Execute();
  }
};

class BltInstruction : public BTypeInstructionInterface {
 public:
  BltInstruction(instr_t instr, RegFilePtr reg_file, PcPtr pc)
      : BTypeInstructionInterface(instr, reg_file, pc) {
    name_ = "blt";
  }

  void Execute() final {
    branch_ = (static_cast<signed_reg_data_t>(Rs1_.second) <
               static_cast<signed_reg_data_t>(Rs2_.second));
    BTypeInstructionInterface::Execute();
  }
};

class BgeInstruction : public BTypeInstructionInterface {
 public:
  BgeInstruction(instr_t instr, RegFilePtr reg_file, PcPtr pc)
      : BTypeInstructionInterface(instr, reg_file, pc) {
    name_ = "bge";
  }

  void Execute() final {
    branch_ = (static_cast<signed_reg_data_t>(Rs1_.second) >=
               static_cast<signed_reg_data_t>(Rs2_.second));
    BTypeInstructionInterface::Execute();
  }
};

class BltuInstruction : public BTypeInstructionInterface {
 public:
  BltuInstruction(instr_t instr, RegFilePtr reg_file, PcPtr pc)
      : BTypeInstructionInterface(instr, reg_file, pc) {
    name_ = "bltu";
  }

  void Execute() final {
    branch_ = (Rs1_.second < Rs2_.second);
    BTypeInstructionInterface::Execute();
  }
};

class BgeuInstruction : public BTypeInstructionInterface {
 public:
  BgeuInstruction(instr_t instr, RegFilePtr reg_file, PcPtr pc)
      : BTypeInstructionInterface(instr, reg_file, pc) {
    name_ = "bgeu";
  }

  void Execute() final {
    branch_ = (Rs1_.second >= Rs2_.second);
    BTypeInstructionInterface::Execute();
  }
};

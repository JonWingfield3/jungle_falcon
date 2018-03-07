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
    reg_file_.Read(Rs1_);

    Rs2_.first = static_cast<RegisterFile::Register>(b_type_format.rs2);
    reg_file_.Read(Rs2_);

    imm_ = static_cast<imm_t>((b_type_format.imm12 << 12) |
                              (b_type_format.imm10_5 << 5) |
                              (b_type_format.imm4_1 << 1));

    dependencies_.set(b_type_format.rs1);
    dependencies_.set(b_type_format.rs2);

    VLOG(2) << name_ << " "
            << "Rs1: " << b_type_format.rs1 << ", Rs2: " << b_type_format.rs2
            << ", Imm: " << imm_;
  }

 protected:
  RegisterFile& reg_file_;
  ProgramCounter& pc_;
  mem_addr_t target_addr_;
  bool branch_;
  RegisterFile::RegDataPair Rs1_;
  RegisterFile::RegDataPair Rs2_;
  imm_t imm_;
};

class BeqInstruction : public BTypeInstructionInterface {
 public:
  BeqInstruction(instr_t instr, RegisterFile& reg_file, ProgramCounter& pc)
      : BTypeInstructionInterface(instr, reg_file, pc) {
    name_ = "Beq";
  }

  void Execute() final {
    branch_ = (Rs1_.second == Rs2_.second);
    VLOG(4) << "Taking BEQ: " << branch_;
  }
};

class BneInstruction : public BTypeInstructionInterface {
 public:
  BneInstruction(instr_t instr, RegisterFile& reg_file, ProgramCounter& pc)
      : BTypeInstructionInterface(instr, reg_file, pc) {
    name_ = "Bne";
  }

  void Execute() final {
    branch_ = (Rs1_.second != Rs2_.second);
    VLOG(4) << "Taking BEQ: " << branch_;
  }
};

class BltInstruction : public BTypeInstructionInterface {
 public:
  BltInstruction(instr_t instr, RegisterFile& reg_file, ProgramCounter& pc)
      : BTypeInstructionInterface(instr, reg_file, pc) {
    name_ = "Blt";
  }

  void Execute() final {
    branch_ = (static_cast<signed_reg_data_t>(Rs1_.second) <
               static_cast<signed_reg_data_t>(Rs2_.second));
    VLOG(4) << "Taking BLT: " << branch_;
  }
};

class BgeInstruction : public BTypeInstructionInterface {
 public:
  BgeInstruction(instr_t instr, RegisterFile& reg_file, ProgramCounter& pc)
      : BTypeInstructionInterface(instr, reg_file, pc) {
    name_ = "Bge";
  }

  void Execute() final {
    branch_ = (static_cast<signed_reg_data_t>(Rs1_.second) >=
               static_cast<signed_reg_data_t>(Rs2_.second));
    VLOG(4) << "Taking BGE: " << branch_;
  }
};

class BltuInstruction : public BTypeInstructionInterface {
 public:
  BltuInstruction(instr_t instr, RegisterFile& reg_file, ProgramCounter& pc)
      : BTypeInstructionInterface(instr, reg_file, pc) {
    name_ = "Bltu";
  }

  void Execute() final {
    branch_ = (Rs1_.second < Rs2_.second);
    VLOG(4) << "Taking BLTU: " << branch_;
  }
};

class BgeuInstruction : public BTypeInstructionInterface {
 public:
  BgeuInstruction(instr_t instr, RegisterFile& reg_file, ProgramCounter& pc)
      : BTypeInstructionInterface(instr, reg_file, pc) {
    name_ = "Bgeu";
  }

  void Execute() final {
    branch_ = (Rs1_.second >= Rs2_.second);
    VLOG(4) << "Taking BGEU: " << branch_;
  }
};

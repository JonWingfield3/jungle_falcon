#pragma once

#include <instructions.hpp>
#include <memory.hpp>
#include <register_file.hpp>
#include <riscv_defs.hpp>

class STypeInstructionInterface : public InstructionInterface {
 public:
  explicit STypeInstructionInterface(instr_t instr, RegisterFile& reg_file,
                                     MemoryPtr mem)
      : InstructionInterface(instr), reg_file_(reg_file), mem_(mem) {
    name_ = "S Type Instruction";
  }

  union PACKED STypeInstructionFormat {
    struct PACKED {
      instr_t opcode : 7;
      instr_t imm4_0 : 5;
      instr_t funct3 : 3;
      instr_t rs1 : 5;
      instr_t rs2 : 5;
      instr_t imm11_5 : 7;
    };
    instr_t word;
  };
  static_assert(sizeof(STypeInstructionFormat) == 4,
                "S Type Instruction size != 4");

  void Decode() {
    STypeInstructionFormat s_type_format;
    s_type_format.word = instr_;

    Rs1_.first = static_cast<RegisterFile::Register>(s_type_format.rs1);
    reg_file_.Read(Rs1_);

    Rs2_.first = static_cast<RegisterFile::Register>(s_type_format.rs2);
    reg_file_.Read(Rs2_);

    imm_ = static_cast<imm_t>((s_type_format.imm11_5 << 5) |
                              (s_type_format.imm4_0));

    dependencies_.set(s_type_format.rs1);
    dependencies_.set(s_type_format.rs2);
  }

  void WriteBack() final {}

 protected:
  RegisterFile& reg_file_;
  MemoryPtr mem_;
  RegisterFile::RegDataPair Rs1_;
  RegisterFile::RegDataPair Rs2_;
  imm_t imm_;
  mem_addr_t store_address_;
};

class SbInstruction : public STypeInstructionInterface {
 public:
  SbInstruction(instr_t instr, RegisterFile& reg_file, MemoryPtr mem)
      : STypeInstructionInterface(instr, reg_file, mem) {
    name_ = "Sb";
  }

  void Execute() final {
    store_address_ = Rs1_.second + imm_;
    VLOG(3) << name_ << ": " << Rs1_.second << " + " << imm_ << " = "
            << store_address_;
  }

  void MemoryAccess() final {
    const uint8_t store_byte = static_cast<uint8_t>(Rs2_.second);
    VLOG(3) << name_ << ": Writing a byte to memory: " << store_byte;
    mem_->Write<uint8_t>(store_address_, store_byte);
  }
};

class ShInstruction : public STypeInstructionInterface {
 public:
  ShInstruction(instr_t instr, RegisterFile& reg_file, MemoryPtr mem)
      : STypeInstructionInterface(instr, reg_file, mem) {
    name_ = "Sh";
  }

  void Execute() final { store_address_ = Rs1_.second + imm_; }

  void MemoryAccess() final {
    const uint16_t store_halfword = static_cast<uint16_t>(Rs2_.second);
    VLOG(3) << name_ << ": Writing a half word to memory: " << store_halfword;
    mem_->Write<uint16_t>(store_address_, store_halfword);
  }
};

class SwInstruction : public STypeInstructionInterface {
 public:
  SwInstruction(instr_t instr, RegisterFile& reg_file, MemoryPtr mem)
      : STypeInstructionInterface(instr, reg_file, mem) {
    name_ = "Sw";
  }

  void Execute() final { store_address_ = Rs1_.second + imm_; }

  void MemoryAccess() final {
    const uint32_t store_word = static_cast<uint32_t>(Rs2_.second);
    VLOG(3) << name_ << ": Writing a word to memory: " << store_word;
    mem_->Write<uint32_t>(store_address_, store_word);
  }
};

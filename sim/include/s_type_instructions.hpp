#pragma once

#include <memory>

#include <instructions.hpp>
#include <memory.hpp>
#include <register_file.hpp>
#include <riscv_defs.hpp>

class STypeInstructionInterface : public InstructionInterface {
 public:
  explicit STypeInstructionInterface(instr_t instr, RegFilePtr reg_file,
                                     MemoryPtr mem)
      : InstructionInterface(instr), reg_file_(reg_file), mem_(mem) {
    name_ = "S Type Instruction";
  }

  ~STypeInstructionInterface() override = default;

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
    reg_file_->Read(Rs1_);

    Rs2_.first = static_cast<RegisterFile::Register>(s_type_format.rs2);
    reg_file_->Read(Rs2_);

    imm_t imm_upper_20 = (((s_type_format.imm11_5 << 5) & (1 << 11)) ? -1 : 0);
    imm_upper_20 &= ~(0xfff);
    imm_ = static_cast<imm_t>(imm_upper_20 | (s_type_format.imm11_5 << 5) |
                              (s_type_format.imm4_0));

    dependencies_.set(s_type_format.rs1);
    dependencies_.set(s_type_format.rs2);

    InstructionInterface::Decode();
  }

  void Execute() final {
    store_address_ = Rs1_.second + imm_;
    VLOG(3) << "Execute: store address calculated as " << std::hex
            << std::showbase << store_address_;
  }

  void MemoryAccess() {
    VLOG(3) << "MemoryAccess: writing" << Rs2_ << " to mem address"
            << store_address_;
  }

  void WriteBack() final { VLOG(3) << "WriteBack stage: Doing nothing"; }

 protected:
  void SetInstructionName() {
    std::stringstream instruction_stream;
    instruction_stream << name_ << " x" << static_cast<int>(Rs2_.first) << ", "
                       << static_cast<int>(imm_) << "(x"
                       << static_cast<int>(Rs1_.first) << ")";
    instruction_ = instruction_stream.str();
  }

  RegFilePtr reg_file_;
  MemoryPtr mem_;
  RegisterFile::RegDataPair Rs1_;
  RegisterFile::RegDataPair Rs2_;
  imm_t imm_;
  mem_addr_t store_address_;
};

class SbInstruction : public STypeInstructionInterface {
 public:
  SbInstruction(instr_t instr, RegFilePtr reg_file, MemoryPtr mem)
      : STypeInstructionInterface(instr, reg_file, mem) {
    name_ = "sb";
  }

  void MemoryAccess() final {
    const uint8_t store_byte = static_cast<uint8_t>(Rs2_.second);
    mem_->WriteByte(store_address_, store_byte);
    STypeInstructionInterface::MemoryAccess();
  }
};

class ShInstruction : public STypeInstructionInterface {
 public:
  ShInstruction(instr_t instr, RegFilePtr reg_file, MemoryPtr mem)
      : STypeInstructionInterface(instr, reg_file, mem) {
    name_ = "sh";
  }

  void MemoryAccess() final {
    const uint16_t store_halfword = static_cast<uint16_t>(Rs2_.second);
    mem_->WriteHalfWord(store_address_, store_halfword);
    STypeInstructionInterface::MemoryAccess();
  }
};

class SwInstruction : public STypeInstructionInterface {
 public:
  SwInstruction(instr_t instr, RegFilePtr reg_file, MemoryPtr mem)
      : STypeInstructionInterface(instr, reg_file, mem) {
    name_ = "sw";
  }

  void MemoryAccess() final {
    const uint32_t store_word = static_cast<uint32_t>(Rs2_.second);
    mem_->WriteWord(store_address_, store_word);
    STypeInstructionInterface::MemoryAccess();
  }
};

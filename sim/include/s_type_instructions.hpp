#pragma once

#include <memory>

#include <instructions.hpp>
#include <memory.hpp>
#include <register_file.hpp>
#include <riscv_defs.hpp>

class STypeInstructionInterface : public InstructionInterface {
 public:
  explicit STypeInstructionInterface(instr_t instr, RegFilePtr reg_file,
                                     MemoryPtr mem);
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

  void Decode();
  void Execute() final;
  void MemoryAccess();
  void WriteBack() final;

  Register& Rs1() { return *Rs1_; }
  Register& Rs2() { return *Rs2_; }
  const Register& Rs1() const { return *Rs1_; }
  const Register& Rs2() const { return *Rs2_; }

 protected:
  void SetInstructionName();
  std::string RegistersString() final;

  RegFilePtr reg_file_;
  MemoryPtr mem_;
  RegPtr Rs1_;
  RegPtr Rs2_;
  imm_t imm_;
  mem_addr_t store_address_;
};

class SbInstruction : public STypeInstructionInterface {
 public:
  SbInstruction(instr_t instr, RegFilePtr reg_file, MemoryPtr mem);
  void MemoryAccess() final;
};

class ShInstruction : public STypeInstructionInterface {
 public:
  ShInstruction(instr_t instr, RegFilePtr reg_file, MemoryPtr mem);
  void MemoryAccess() final;
};

class SwInstruction : public STypeInstructionInterface {
 public:
  SwInstruction(instr_t instr, RegFilePtr reg_file, MemoryPtr mem);
  void MemoryAccess() final;
};

#pragma once

#include <cstdint>

#include <bitset>
#include <memory>
#include <register_file.hpp>
#include <riscv_defs.hpp>

#define PACKED __attribute__((__packed__))

enum class OpCode {
  LUI = 0b0110111,
  AUIPC = 0b0010111,
  JAL = 0b1101111,
  JALR = 0b1100111,
  Bxx = 0b1100011,  // Branch instructions Op
  Lx = 0b0000011,   // Load instructions Op
  Sx = 0b0100011,   // Store instructions Op
  ITypeArithmeticAndLogical = 0b0010011,
  RTypeArithmeticAndLogical = 0b0110011,
};

enum class InstrTypes { UType, Jtype, IType, BType, RType, SType };

enum class Funct3 {
  BEQ = 0b000,
  BNE = 0b001,
  BLT = 0b100,
  BGE = 0b101,
  BLTU = 0b110,
  BGEU = 0b111,
  LB = 0b000,
  LH = 0b001,
  LW = 0b010,
  LBU = 0b100,
  SB = 0b000,
  SH = 0b001,
  SW = 0b010,
  ADDI = 0b000,
  SLTI = 0b010,
  SLTIU = 0b011,
  XORI = 0b100,
  ORI = 0b110,
  ANDI = 0b111,
  SLLI = 0b001,
  SRLI = 0b101,
  SRAI = 0b101,
  ADD = 0b000,
  SUB = 0b000,
  SLL = 0b001,
  SLT = 0b010,
  SLTU = 0b011,
  XOR = 0b100,
  SRL = 0b101,
  SRA = 0b101,
  OR = 0b110,
  AND = 0b111,
  FENCE = 0b000,
  FENCEI = 0b001,
};

enum class Funct7 {
  XXX = 0,
  SRLI = 0b0000000,
  SRAI = 0b0100000,
  ADD = 0b0000000,
  SUB = 0b0100000,
  SRL = 0b0000000,
  SRA = 0b0100000
};

class InstructionInterface {
 public:
  explicit InstructionInterface(instr_t instr) : instr_(instr) {}

  union PACKED GenericInstructionFormat {
    struct PACKED {
      instr_t opcode : 7;
      instr_t upper : 25;
    };
    instr_t word;
  };

  static_assert(sizeof(GenericInstructionFormat) == 4,
                "Generic Instruction size != 4");

  virtual void Run() {
#if defined(__INSTRUCTION_ACCURATE__)
    Decode();
    Execute();
    MemoryAccess();
    WriteBack();
#elif defined(__CYCLE_ACCURATE__)

#endif
  }

  virtual void Decode() = 0;
  virtual void Execute() = 0;
  virtual void MemoryAccess() = 0;
  virtual void WriteBack() = 0;

  RegisterFile::RegisterMask Dependencies() { return dependencies_; }

 protected:
  size_t current_stage_;
  std::string name_;
  instr_t instr_;
  InstrTypes instr_type_;
  RegisterFile::RegisterMask dependencies_;
};

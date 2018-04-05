#pragma once

#include <memory>

#include <instructions.hpp>
#include <register_file.hpp>
#include <riscv_defs.hpp>

class UTypeInstructionInterface : public InstructionInterface {
 public:
  explicit UTypeInstructionInterface(instr_t instr, RegFilePtr reg_file);
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

  void Decode();
  void Execute();
  void WriteBack() final;

  Register& Rd() { return *Rd_; }
  const Register& Rd() const { return *Rd_; }

 protected:
  void SetInstructionName();
  std::string RegistersString() final;

  RegFilePtr reg_file_;
  RegPtr Rd_;
  imm_t imm_;
};

class LuiInstruction : public UTypeInstructionInterface {
 public:
  LuiInstruction(instr_t instr, RegFilePtr reg_file);
  void Execute() final;
};

class AuipcInstruction : public UTypeInstructionInterface {
 public:
  AuipcInstruction(instr_t instr, RegFilePtr reg_file, PcPtr pc);
  void Execute() final;

 private:
  PcPtr pc_;
};

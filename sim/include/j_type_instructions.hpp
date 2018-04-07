#pragma once

#include <memory>

#include <instructions.hpp>
#include <register_file.hpp>
#include <riscv_defs.hpp>

class JalInstruction : public InstructionInterface {
 public:
  explicit JalInstruction(instr_t instr, RegFilePtr reg_file, PcPtr pc);

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

  void Decode() final;
  void Execute() final;
  void MemoryAccess() final;
  void WriteBack() final;

  Register& Rd() { return *Rd_; }
  const Register& Rd() const { return *Rd_; }

  OpCode GetOpCode() const final { return OpCode::JAL; }

 protected:
  void SetInstructionName();
  std::string RegistersString() final;

  RegFilePtr reg_file_;
  PcPtr pc_;
  RegPtr Rd_;
  imm_t imm_;
};

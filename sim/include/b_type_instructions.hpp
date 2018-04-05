#pragma once

#include <memory>

#include <instructions.hpp>
#include <register_file.hpp>
#include <riscv_defs.hpp>

class BTypeInstructionInterface : public InstructionInterface {
 public:
  explicit BTypeInstructionInterface(instr_t instr, RegFilePtr reg_file,
                                     PcPtr& pc);
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

  void Decode();
  void Execute();
  void WriteBack() final;

  Register& Rs1() { return *Rs1_; }
  Register& Rs2() { return *Rs2_; }
  const Register& Rs1() const { return *Rs1_; }
  const Register& Rs2() const { return *Rs2_; }

  bool WillBranch() { return branch_; }

 protected:
  void SetInstructionName();
  std::string RegistersString() final;

  RegFilePtr reg_file_;
  PcPtr pc_;
  mem_addr_t target_addr_;
  bool branch_;
  RegPtr Rs1_;
  RegPtr Rs2_;
  int imm_;
};

class BeqInstruction : public BTypeInstructionInterface {
 public:
  BeqInstruction(instr_t instr, RegFilePtr reg_file, PcPtr pc);
  void Execute() final;
};

class BneInstruction : public BTypeInstructionInterface {
 public:
  BneInstruction(instr_t instr, RegFilePtr reg_file, PcPtr pc);
  void Execute() final;
};

class BltInstruction : public BTypeInstructionInterface {
 public:
  BltInstruction(instr_t instr, RegFilePtr reg_file, PcPtr pc);
  void Execute() final;
};

class BgeInstruction : public BTypeInstructionInterface {
 public:
  BgeInstruction(instr_t instr, RegFilePtr reg_file, PcPtr pc);
  void Execute() final;
};

class BltuInstruction : public BTypeInstructionInterface {
 public:
  BltuInstruction(instr_t instr, RegFilePtr reg_file, PcPtr pc);
  void Execute() final;
};

class BgeuInstruction : public BTypeInstructionInterface {
 public:
  BgeuInstruction(instr_t instr, RegFilePtr reg_file, PcPtr pc);
  void Execute() final;
};

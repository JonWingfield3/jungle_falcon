#pragma once

#include <memory>

#include <instructions.hpp>
#include <register_file.hpp>
#include <riscv_defs.hpp>

class RTypeInstructionInterface : public InstructionInterface {
 public:
  explicit RTypeInstructionInterface(instr_t instr, RegFilePtr reg_file);
  ~RTypeInstructionInterface() override = default;

  union PACKED RTypeInstructionFormat {
    struct PACKED {
      instr_t opcode : 7;
      instr_t rd : 5;
      instr_t funct3 : 3;
      instr_t rs1 : 5;
      instr_t rs2 : 5;
      instr_t funct7 : 7;
    };
    instr_t word;
  };
  static_assert(sizeof(RTypeInstructionFormat) == 4,
                "R Type Instruction size != 4");

  void Decode() final;
  void Execute();
  void MemoryAccess() final;
  void WriteBack() final;

  Register& Rd() { return *Rd_; }
  Register& Rs1() { return *Rs1_; }
  Register& Rs2() { return *Rs2_; }

  const Register& Rd() const { return *Rd_; }
  const Register& Rs1() const { return *Rs1_; }
  const Register& Rs2() const { return *Rs2_; }

  OpCode GetOpCode() const final { return OpCode::RTypeArithmeticAndLogical; }

 protected:
  void SetInstructionName();
  std::string RegistersString() final;

  RegFilePtr reg_file_;
  RegPtr Rd_;
  RegPtr Rs1_;
  RegPtr Rs2_;
};

class SxxiInstructionInterface : public RTypeInstructionInterface {
 public:
  SxxiInstructionInterface(instr_t instr, RegFilePtr reg_file);
  void Execute();

 protected:
  void SetInstructionName() final;
  int shamt_;
};

class SlliInstruction : public SxxiInstructionInterface {
 public:
  SlliInstruction(instr_t instr, RegFilePtr reg_file);
  void Execute() final;
};

class SrliInstruction : public SxxiInstructionInterface {
 public:
  SrliInstruction(instr_t instr, RegFilePtr reg_file);
  void Execute() final;
};

class SraiInstruction : public SxxiInstructionInterface {
 public:
  SraiInstruction(instr_t instr, RegFilePtr reg_file);
  void Execute() final;
};

class AddInstruction : public RTypeInstructionInterface {
 public:
  AddInstruction(instr_t instr, RegFilePtr reg_file);
  void Execute() final;
};

class SubInstruction : public RTypeInstructionInterface {
 public:
  SubInstruction(instr_t instr, RegFilePtr reg_file);
  void Execute() final;
};

class SllInstruction : public RTypeInstructionInterface {
 public:
  SllInstruction(instr_t instr, RegFilePtr reg_file);
  void Execute() final;
};

class SltInstruction : public RTypeInstructionInterface {
 public:
  SltInstruction(instr_t instr, RegFilePtr reg_file);
  void Execute() final;
};

class SltuInstruction : public RTypeInstructionInterface {
 public:
  SltuInstruction(instr_t instr, RegFilePtr reg_file);
  void Execute() final;
};

class XorInstruction : public RTypeInstructionInterface {
 public:
  XorInstruction(instr_t instr, RegFilePtr reg_file);
  void Execute() final;
};

class SrlInstruction : public RTypeInstructionInterface {
 public:
  SrlInstruction(instr_t instr, RegFilePtr reg_file);
  void Execute() final;
};

class SraInstruction : public RTypeInstructionInterface {
 public:
  SraInstruction(instr_t instr, RegFilePtr reg_file);
  void Execute() final;
};

class OrInstruction : public RTypeInstructionInterface {
 public:
  OrInstruction(instr_t instr, RegFilePtr reg_file);
  void Execute() final;
};

class AndInstruction : public RTypeInstructionInterface {
 public:
  AndInstruction(instr_t instr, RegFilePtr reg_file);
  void Execute() final;
};

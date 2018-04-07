#pragma once

#include <functional>
#include <memory>

#include <instructions.hpp>
#include <memory.hpp>
#include <register_file.hpp>
#include <riscv_defs.hpp>

class ITypeInstructionInterface : public InstructionInterface {
 public:
  explicit ITypeInstructionInterface(instr_t instr, RegFilePtr reg_file);
  ~ITypeInstructionInterface() override = default;

  union PACKED ITypeInstructionFormat {
    struct PACKED {
      instr_t opcode : 7;
      instr_t rd : 5;
      instr_t funct3 : 3;
      instr_t rs1 : 5;
      instr_t imm11_0 : 12;
    };
    instr_t word;
  };
  static_assert(sizeof(ITypeInstructionFormat) == 4,
                "I Type Instruction size != 4");

  void Decode() final;
  void Execute();
  void WriteBack();

  Register& Rd() { return *Rd_; }
  Register& Rs1() { return *Rs1_; }
  const Register& Rd() const { return *Rd_; }
  const Register& Rs1() const { return *Rs1_; }

  OpCode GetOpCode() const { return OpCode::ITypeArithmeticAndLogical; }

 protected:
  void SetInstructionName();
  std::string RegistersString() final;

  RegFilePtr reg_file_;
  RegPtr Rd_;
  RegPtr Rs1_;
  imm_t imm_;
};

class AddiInstruction : public ITypeInstructionInterface {
 public:
  AddiInstruction(instr_t instr, RegFilePtr reg_file);
  void Execute() final;
};

class SltiInstruction : public ITypeInstructionInterface {
 public:
  SltiInstruction(instr_t instr, RegFilePtr reg_file);
  void Execute() final;
};

class SltiuInstruction : public ITypeInstructionInterface {
 public:
  SltiuInstruction(instr_t instr, RegFilePtr reg_file);
  void Execute() final;
};

class XoriInstruction : public ITypeInstructionInterface {
 public:
  XoriInstruction(instr_t instr, RegFilePtr reg_file);
  void Execute() final;
};

class OriInstruction : public ITypeInstructionInterface {
 public:
  OriInstruction(instr_t instr, RegFilePtr reg_file);
  void Execute() final;
};

class AndiInstruction : public ITypeInstructionInterface {
 public:
  AndiInstruction(instr_t instr, RegFilePtr reg_file);
  void Execute() final;
};

class JalrInstruction : public ITypeInstructionInterface {
 public:
  JalrInstruction(instr_t instr, RegFilePtr reg_file, PcPtr pc);

  void Execute() final;
  void MemoryAccess() final;
  void WriteBack() final;

  OpCode GetOpCode() const final { return OpCode::JALR; }

 private:
  PcPtr pc_;
  int target_addr_;
};

class LoadInstructionInterface : public ITypeInstructionInterface {
 public:
  LoadInstructionInterface(instr_t instr, RegFilePtr reg_file, MemoryPtr mem);

  void Execute() final;
  void MemoryAccess();
  void WriteBack() final;

  OpCode GetOpCode() const final { return OpCode::Lx; }

 protected:
  void SetInstructionName() final;

  MemoryPtr mem_;
  mem_addr_t load_addr_;
};

class LbInstruction : public LoadInstructionInterface {
 public:
  LbInstruction(instr_t instr, RegFilePtr reg_file, MemoryPtr mem);
  void MemoryAccess() final;
};

class LbuInstruction : public LoadInstructionInterface {
 public:
  LbuInstruction(instr_t instr, RegFilePtr reg_file, MemoryPtr mem);
  void MemoryAccess() final;
};

class LhInstruction : public LoadInstructionInterface {
 public:
  LhInstruction(instr_t instr, RegFilePtr reg_file, MemoryPtr mem);
  void MemoryAccess() final;
};

class LhuInstruction : public LoadInstructionInterface {
 public:
  LhuInstruction(instr_t instr, RegFilePtr reg_file, MemoryPtr mem);
  void MemoryAccess() final;
};

class LwInstruction : public LoadInstructionInterface {
 public:
  LwInstruction(instr_t instr, RegFilePtr reg_file, MemoryPtr mem);
  void MemoryAccess() final;
};

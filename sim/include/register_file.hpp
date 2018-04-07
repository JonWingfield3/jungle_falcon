#pragma once

#include <bitset>
#include <cstdint>
#include <iostream>
#include <memory>
#include <vector>

#include <riscv_defs.hpp>

class RegisterFile;
using RegFilePtr = std::shared_ptr<RegisterFile>;

class ProgramCounter;
using PcPtr = std::shared_ptr<ProgramCounter>;

class Register;
using RegPtr = std::shared_ptr<Register>;

class Register {
 public:
  Register();
  Register(int reg_num, reg_data_t data = 0);

  const reg_data_t& Data() const;
  reg_data_t& Data();

  int Number() const;
  void Clear();

  friend std::ostream& operator<<(std::ostream& stream, const Register& reg);

 private:
  reg_data_t data_;
  int reg_num_;
};

class RegisterFile {
 public:
  RegisterFile();

  enum class Registers {
    X0,
    X1,
    LR = X1,
    X2,  // stack pointer
    SP = X2,
    X3,  // global pointer
    GP = X3,
    X4,  // thread pointer
    TP = X4,
    X5,  // Temp 0
    Temp0 = X5,
    X6,  // Temp 1
    Temp1 = X6,
    X7,  // Temp2
    Temp2 = X7,
    X8,
    X9,
    X10,
    X11,
    X12,
    X13,
    X14,
    X15,
    X16,
    X17,
    X18,
    X19,
    X20,
    X21,
    X22,
    X23,
    X24,
    X25,
    X26,
    X27,
    X28,
    X29,
    X30,
    X31,
  };
  static constexpr int NumCPURegisters = 32;

  reg_data_t Read(Registers reg) const;
  void Read(Register& reg) const;

  void Write(Registers reg, reg_data_t write_data);
  void Write(const Register& reg);

  void DumpRegisters() const;

  void Reset();

 private:
  std::vector<Register> registers_;
};

class ProgramCounter {
 public:
  ProgramCounter(mem_addr_t entry_point = 0);

  ProgramCounter& operator++();
  ProgramCounter& operator+=(mem_offset_t offset);

  mem_addr_t Reg() const;

  void Jump(mem_addr_t jump_addr);
  void BranchOffset(int offset);
  void Reset();

  friend std::ostream& operator<<(std::ostream& stream,
                                  const ProgramCounter& pc);

 private:
  mem_addr_t program_counter_ = 0;
};

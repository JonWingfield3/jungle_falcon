#pragma once

#include <array>
#include <bitset>
#include <cstdint>
#include <iostream>
#include <memory>

#include <memory.hpp>
#include <riscv_defs.hpp>

class RegisterFile;
using RegFilePtr = std::shared_ptr<RegisterFile>;

class ProgramCounter;
using PcPtr = std::shared_ptr<ProgramCounter>;

class RegisterFile {
 public:
  RegisterFile();

  enum class Register {
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

  using RegDataPair = std::pair<RegisterFile::Register, reg_data_t>;
  using RegisterMask = std::bitset<RegisterFile::NumCPURegisters>;

  reg_data_t Read(Register reg) const;
  void Read(RegDataPair& reg_data_pair) const {
    reg_data_pair.second = Read(reg_data_pair.first);
  }

  void Write(Register reg, reg_data_t write_data);
  void Write(RegDataPair reg_data_pair) {
    Write(reg_data_pair.first, reg_data_pair.second);
  }

  void DumpRegisters(std::vector<Register> registers);

  void Reset() { registers_.fill(0); }

  friend std::ostream& operator<<(std::ostream& stream,
                                  RegDataPair reg_data_pair) {
    stream << "(x" << static_cast<int>(reg_data_pair.first) << ", "
           << reg_data_pair.second << ")";
  }

 private:
  std::array<reg_data_t, NumCPURegisters> registers_;
};

class ProgramCounter {
 public:
  ProgramCounter(mem_addr_t entry_point = 0) : program_counter_(entry_point) {}

  ProgramCounter& operator++() {
    program_counter_ += sizeof(instr_t);
    return *this;
  }

  ProgramCounter& operator+=(mem_offset_t offset) {
    program_counter_ += offset;
  }

  mem_addr_t Reg() const { return program_counter_; }

  void Branch(int offset) {
    const int signed_pc = offset - 4 + static_cast<int>(program_counter_);
    CHECK(signed_pc >= 0) << "PC went negative: " << signed_pc;
    program_counter_ = static_cast<reg_data_t>(signed_pc);
  }

  void Jump(mem_addr_t jump_addr) {
    program_counter_ = jump_addr - 4;
    VLOG(4) << "Jumping to address " << jump_addr;
  }

  void Reset() { program_counter_ = 0; }

 private:
  mem_addr_t program_counter_ = 0;
};

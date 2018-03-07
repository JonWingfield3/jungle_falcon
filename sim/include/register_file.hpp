#pragma once

#include <array>
#include <cstdint>

#include <memory.hpp>
#include <riscv_defs.hpp>

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

  reg_data_t Read(Register reg) const;
  void Write(Register reg, reg_data_t write_data);

 private:
  std::array<reg_data_t, NumCPURegisters> registers_;
};

class ProgramCounter {
 public:
  ProgramCounter(mem_addr_t entry_point = 0) : program_counter_(entry_point) {}

  ProgramCounter& operator++() {
    program_counter_ += 4;
    return *this;
  }

  ProgramCounter& operator+=(mem_offset_t offset) {
    program_counter_ += offset;
  }

  mem_addr_t pc() const { return program_counter_; }

  //  ProgramCounter& operator=(mem_addr_t mem_addr) {
  //    this->program_counter_ = mem_addr;
  //    return *this;
  //  }

 private:
  mem_addr_t program_counter_ = 0;
};

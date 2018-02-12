#pragma once

#include <array>
#include <cstdint>

class RegisterFile {
 public:
  RegisterFile();
  ~RegisterFile();

  static constexpr int NUM_CPU_REGISTERS = 32;

  enum Register {
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
    X31
  };

  uint64_t Read(Register reg) const;
  void Write(Register reg, uint64_t write_val);

 private:
  friend class RegisterFileTests;
  std::array<uint64_t, NUM_CPU_REGISTERS> registers_;
};

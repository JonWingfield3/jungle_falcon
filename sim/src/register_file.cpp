#include <register_file.hpp>

#include <array>
#include <cstdint>
#include <iostream>

RegisterFile::RegisterFile() { registers_.fill(0); }

RegisterFile::~RegisterFile() {}

uint32_t RegisterFile::Read(Register reg) const {
  if (reg == X0) {
    return 0;
  } else {
    return registers_.at(static_cast<unsigned>(reg));
  }
}

void RegisterFile::Write(Register reg, uint32_t write_val) {
  if (reg == X0) {
    std::cerr << "Writing to X0!" << std::endl;
  } else {
    registers_.at(static_cast<unsigned>(reg)) = write_val;
  }
}

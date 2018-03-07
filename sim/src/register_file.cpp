#include <register_file.hpp>

#include <array>
#include <cstdint>
#include <iostream>

RegisterFile::RegisterFile() { registers_.fill(0); }

reg_data_t RegisterFile::Read(Register reg) const {
  const reg_data_t reg_data = registers_.at(static_cast<unsigned>(reg));
  VLOG(4) << std::hex << std::showbase << "Read " << reg_data << " from reg "
          << static_cast<int>(reg);
  return reg_data;
}

void RegisterFile::Write(Register reg, reg_data_t write_data) {
  CHECK(reg != Register::X0) << "Write to X0!";
  registers_.at(static_cast<unsigned>(reg)) = write_data;
  VLOG(4) << std::hex << std::showbase << "Wrote " << write_data << " to reg "
          << static_cast<int>(reg);
}

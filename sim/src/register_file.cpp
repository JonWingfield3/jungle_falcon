#include <register_file.hpp>

#include <array>
#include <cstdint>
#include <iomanip>
#include <iostream>

RegisterFile::RegisterFile() { registers_.fill(0); }

reg_data_t RegisterFile::Read(Register reg) const {
  const reg_data_t reg_data = registers_.at(static_cast<unsigned>(reg));
  VLOG(5) << std::hex << std::showbase << "Read " << reg_data << " from reg "
          << static_cast<int>(reg);
  return reg_data;
}

void RegisterFile::Write(Register reg, reg_data_t write_data) {
  if ((reg == Register::X0) && (write_data != 0)) return;
  registers_.at(static_cast<unsigned>(reg)) = write_data;
  VLOG(5) << std::hex << std::showbase << "Wrote " << write_data << " to reg "
          << static_cast<int>(reg);
}

void RegisterFile::DumpRegisters(std::vector<Register> registers) {
  static constexpr int kRegisterDumpWidth = 4;
  int ii = 0;
  for (const auto reg : registers) {
    const int reg_int = static_cast<int>(reg);
    std::cout << "|x" << std::dec << std::setfill('0') << std::setw(2)
              << reg_int << ": " << std::setw(0) << std::showbase << std::hex
              << std::setw(10) << std::setfill(' ') << registers_.at(reg_int);
    if (++ii % kRegisterDumpWidth) {
      std::cout << "\t";
    } else {
      std::cout << std::endl;
    }
  }
}

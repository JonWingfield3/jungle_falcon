#include <register_file.hpp>

#include <array>
#include <cstdint>
#include <iomanip>
#include <iostream>

RegisterFile::RegisterFile() {
  for (int ii = 0; ii < NumCPURegisters; ++ii) {
    registers_.push_back(Register(ii, 0));
  }
}

reg_data_t RegisterFile::Read(Registers reg) const {
  const auto& read_reg = registers_.at(static_cast<unsigned>(reg));
  const reg_data_t reg_data = read_reg.Data();
  VLOG(5) << std::hex << std::showbase << "Read " << reg_data << " from reg "
          << static_cast<int>(reg);
  return reg_data;
}

void RegisterFile::Write(Registers reg, reg_data_t write_data) {
  if ((reg == Registers::X0) && (write_data != 0)) return;
  registers_.at(static_cast<unsigned>(reg)).Data() = write_data;
  VLOG(5) << std::hex << std::showbase << "Wrote " << write_data << " to reg "
          << static_cast<int>(reg);
}

void RegisterFile::DumpRegisters(std::vector<Registers> registers) {
  static constexpr int kRegisterDumpWidth = 4;
  int ii = 0;
  for (const auto& reg : registers_) {
    std::cout << reg;
    if (++ii % kRegisterDumpWidth) {
      std::cout << "\t";
    } else {
      std::cout << std::endl;
    }
  }
}

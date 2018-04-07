#include <register_file.hpp>

#include <array>
#include <cstdint>
#include <iomanip>
#include <iostream>

#include <memory.hpp>
#include <pipeline.hpp>
#include <riscv_defs.hpp>

////////////////////////////////////////////////////////////////////////////////
Register::Register() : data_(0), reg_num_(0) {}

////////////////////////////////////////////////////////////////////////////////
Register::Register(int reg_num, reg_data_t data)
    : data_(data), reg_num_(reg_num) {}

////////////////////////////////////////////////////////////////////////////////
const reg_data_t& Register::Data() const { return data_; }

////////////////////////////////////////////////////////////////////////////////
reg_data_t& Register::Data() { return data_; }

////////////////////////////////////////////////////////////////////////////////
int Register::Number() const { return reg_num_; }

////////////////////////////////////////////////////////////////////////////////
void Register::Clear() { data_ = 0; }

////////////////////////////////////////////////////////////////////////////////
std::ostream& operator<<(std::ostream& stream, const Register& reg) {
  stream << std::dec << "{x" << std::setw(2) << std::setfill('0')
         << reg.reg_num_ << ": 0x" << std::hex << std::setw(8)
         << std::setfill('0') << reg.data_ << "}";
  return stream;
}

////////////////////////////////////////////////////////////////////////////////
RegisterFile::RegisterFile() {
  for (int ii = 0; ii < NumCPURegisters; ++ii) {
    registers_.push_back(Register(ii, 0));
  }
}

////////////////////////////////////////////////////////////////////////////////
reg_data_t RegisterFile::Read(Registers reg) const {
  const auto& read_reg = registers_.at(static_cast<unsigned>(reg));
  const reg_data_t reg_data = read_reg.Data();
  VLOG(5) << std::hex << std::showbase << "Read " << reg_data << " from reg "
          << static_cast<int>(reg);
  return reg_data;
}

////////////////////////////////////////////////////////////////////////////////
void RegisterFile::Write(Registers reg, reg_data_t write_data) {
  if ((reg == Registers::X0) && (write_data != 0)) return;
  registers_.at(static_cast<unsigned>(reg)).Data() = write_data;
  VLOG(5) << std::hex << std::showbase << "Wrote " << write_data << " to reg "
          << static_cast<int>(reg);
}

////////////////////////////////////////////////////////////////////////////////
void RegisterFile::DumpRegisters() const {
  static constexpr int kRegisterDumpWidth = 4;
  int ii = 0;
  for (const auto& reg : registers_) {
    std::cout << reg;
    if (++ii % kRegisterDumpWidth) {
      std::cout << "\t\t";
    } else {
      std::cout << std::endl;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
void RegisterFile::Read(Register& reg) const {
  reg.Data() = Read(static_cast<Registers>(reg.Number()));
}

////////////////////////////////////////////////////////////////////////////////
void RegisterFile::Write(const Register& reg) {
  Write(static_cast<Registers>(reg.Number()), reg.Data());
}

////////////////////////////////////////////////////////////////////////////////
void RegisterFile::Reset() {
  for (auto& reg : registers_) {
    reg.Clear();
  }
}

////////////////////////////////////////////////////////////////////////////////
ProgramCounter::ProgramCounter(mem_addr_t entry_point)
    : program_counter_(entry_point) {}

////////////////////////////////////////////////////////////////////////////////
ProgramCounter& ProgramCounter::operator++() {
  program_counter_ += sizeof(instr_t);
  return *this;
}

////////////////////////////////////////////////////////////////////////////////
ProgramCounter& ProgramCounter::operator+=(mem_offset_t offset) {
  program_counter_ += offset;
  return *this;
}

////////////////////////////////////////////////////////////////////////////////
mem_addr_t ProgramCounter::Reg() const { return program_counter_; }

////////////////////////////////////////////////////////////////////////////////
void ProgramCounter::BranchOffset(int offset) {
#if (__INSTRUCTION_ACCURATE__ == 1)
  const int pipeline_offset = sizeof(instr_t);
#else
  const int pipeline_offset = ((Pipeline::Stages::MemoryAccessStage -
                                Pipeline::Stages::FetchStage + 1) *
                               sizeof(instr_t));
#endif
  const int branch_offset = offset - pipeline_offset;
  const int signed_pc = branch_offset + static_cast<int>(program_counter_);
  CHECK(signed_pc >= 0) << "PC went negative: " << signed_pc;
  program_counter_ = static_cast<reg_data_t>(signed_pc);
}

////////////////////////////////////////////////////////////////////////////////
void ProgramCounter::Jump(mem_addr_t jump_addr) {
#if (__INSTRUCTION_ACCURATE__ == 1)
  const int signed_pc = jump_addr - sizeof(instr_t);
#else
  const int signed_pc = jump_addr - (4 * sizeof(instr_t));
#endif

  CHECK(signed_pc >= 0) << "PC went negative: " << signed_pc;
  program_counter_ = static_cast<reg_data_t>(signed_pc);
}

////////////////////////////////////////////////////////////////////////////////
void ProgramCounter::Reset() { program_counter_ = 0; }

////////////////////////////////////////////////////////////////////////////////
std::ostream& operator<<(std::ostream& stream, const ProgramCounter& pc) {
  stream << std::hex << std::showbase << static_cast<int>(pc.Reg());
  return stream;
}

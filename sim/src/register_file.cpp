#include <register_file.hpp>

#include <array>
#include <cstdint>
#include <iomanip>
#include <iostream>

#include <memory.hpp>
#include <pipeline.hpp>
#include <riscv_defs.hpp>

////////////////////////////////////////////////////////////////////////////////
Register::Register() : HardwareObject(), data_(0), reg_num_(0) {}

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
void Register::Reset() {
  data_ = 0;
  HardwareObject::Reset();
}

////////////////////////////////////////////////////////////////////////////////
std::ostream& operator<<(std::ostream& stream, const Register& reg) {
  stream << std::dec << "{x" << std::setw(2) << std::setfill('0')
         << reg.reg_num_ << ": 0x" << std::hex << std::setw(8)
         << std::setfill('0') << reg.data_ << "}";
  return stream;
}

////////////////////////////////////////////////////////////////////////////////
RegisterFile::RegisterFile() : HardwareObject() {
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
void ProgramCounter::ExecuteCycle() {
  instruction_pointer_ += sizeof(instr_t);
  HardwareObject::ExecuteCycle();
}

////////////////////////////////////////////////////////////////////////////////
void RegisterFile::Reset() {
  for (auto& reg : registers_) {
    reg.Reset();
  }
  HardwareObject::Reset();
}

////////////////////////////////////////////////////////////////////////////////
ProgramCounter::ProgramCounter(mem_addr_t entry_point)
    : HardwareObject(), instruction_pointer_(entry_point) {}

////////////////////////////////////////////////////////////////////////////////
mem_addr_t ProgramCounter::InstructionPointer() const {
  return instruction_pointer_;
}

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
  const int signed_pc = branch_offset + static_cast<int>(instruction_pointer_);
  CHECK(signed_pc >= 0) << "PC went negative: " << signed_pc;
  instruction_pointer_ = static_cast<reg_data_t>(signed_pc);
}

////////////////////////////////////////////////////////////////////////////////
void ProgramCounter::Jump(mem_addr_t jump_addr) {
#if (__INSTRUCTION_ACCURATE__ == 1)
  const int signed_pc = jump_addr - sizeof(instr_t);
#else
  const int signed_pc = jump_addr - (4 * sizeof(instr_t));
#endif

  CHECK(signed_pc >= 0) << "PC went negative: " << signed_pc;
  instruction_pointer_ = static_cast<reg_data_t>(signed_pc);
}

////////////////////////////////////////////////////////////////////////////////
void ProgramCounter::Reset() {
  instruction_pointer_ = 0;
  HardwareObject::Reset();
}

////////////////////////////////////////////////////////////////////////////////
std::ostream& operator<<(std::ostream& stream, const ProgramCounter& pc) {
  stream << std::hex << std::showbase
         << static_cast<int>(pc.InstructionPointer());
  return stream;
}

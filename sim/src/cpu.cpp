#include <cpu.hpp>

#include <algorithm>

#include <instructions.hpp>
#include <register_file.hpp>

////////////////////////////////////////////////////////////////////////////////
CPU::CPU(MemoryPtr instr_mem, MemoryPtr data_mem)
    : instr_mem_(instr_mem), data_mem_(data_mem) {
  reg_file_ = std::make_shared<RegisterFile>(RegisterFile());
  pc_ = std::make_shared<ProgramCounter>(ProgramCounter());
  pipeline_ = std::make_shared<Pipeline>(
      Pipeline(reg_file_, pc_, instr_mem_, data_mem_));
  data_hazard_detector_ = std::make_shared<DataHazardDetectionUnit>(
      DataHazardDetectionUnit(pipeline_));
  control_hazard_detector_ = std::make_shared<ControlHazardDetectionUnit>(
      ControlHazardDetectionUnit(pipeline_));
}

////////////////////////////////////////////////////////////////////////////////
void CPU::Reset() {
  reg_file_->Reset();
  pc_->Reset();
  pipeline_->Flush();
}

////////////////////////////////////////////////////////////////////////////////
double CPU::GetCPI() const {
  const std::size_t instructions_completed = pipeline_->InstructionsCompleted();
  if (instructions_completed == 0) {
    return 0.0;
  } else {
    return (double)cycles_ / (double)instructions_completed;
  }
}

////////////////////////////////////////////////////////////////////////////////
void CPU::ExecuteCycle(std::size_t n) {
  for (std::size_t ii = 0; ii < n; ++ii) {
    if ((ii != 0) && std::any_of(bkpts_.cbegin(), bkpts_.cend(),
                                 [&](const Breakpoint& bkpt) {
                                   return bkpt.second == pc_->Reg();
                                 })) {
      VLOG(5) << "Hit breakpoint";
      break;
    }
    pipeline_->ExecuteCycle();
    control_hazard_detector_->HandleHazard();
    data_hazard_detector_->HandleHazard();
    ++cycles_;
  }
}

////////////////////////////////////////////////////////////////////////////////
void CPU::SetBreakpoint(mem_addr_t breakpoint_address) {
  bkpts_.push_back(
      std::make_pair(static_cast<int>(bkpts_.size()), breakpoint_address));
}

////////////////////////////////////////////////////////////////////////////////
void CPU::DeleteBreakpoint(int bkpt_num) {
  bkpts_.erase(
      std::remove_if(bkpts_.begin(), bkpts_.end(),
                     [&](Breakpoint& bkpt) { return bkpt.first == bkpt_num; }),
      bkpts_.end());
}

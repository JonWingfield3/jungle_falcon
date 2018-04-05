#include <cpu.hpp>

#include <algorithm>

#include <instructions.hpp>
#include <register_file.hpp>

////////////////////////////////////////////////////////////////////////////////
CPU::CPU(MemoryPtr mem) : mem_(mem) {
  reg_file_ = std::make_shared<RegisterFile>(RegisterFile());
  pc_ = std::make_shared<ProgramCounter>(ProgramCounter());
  pipeline_ = std::make_shared<Pipeline>(Pipeline(reg_file_, pc_, mem_));
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

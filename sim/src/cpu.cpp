#include <cpu.hpp>

#include <algorithm>

#include <instructions.hpp>
#include <register_file.hpp>

void CPU::Reset() {
  reg_file_->Reset();
  pc_->Reset();
  pipeline_->Reset();
}

void CPU::ExecuteCycle(std::size_t n) {
  for (std::size_t ii = 0; ii < n; ++ii) {
    if ((ii != 0) && std::any_of(bkpts_.cbegin(), bkpts_.cend(),
                                 [&](const Breakpoint& bkpt) {
                                   return bkpt.second == pc_->Reg();
                                 })) {
      VLOG(3) << "Hit breakpoint";
      break;
    }
    pipeline_->ExecuteCycle();
    data_hazard_detector_->HandleHazard();
  }
}

void CPU::SetBreakpoint(mem_addr_t breakpoint_address) {
  bkpts_.push_back(
      std::make_pair(static_cast<int>(bkpts_.size()), breakpoint_address));
}

void CPU::DeleteBreakpoint(int bkpt_num) {
  bkpts_.erase(
      std::remove_if(bkpts_.begin(), bkpts_.end(),
                     [&](Breakpoint& bkpt) { return bkpt.first == bkpt_num; }),
      bkpts_.end());
}

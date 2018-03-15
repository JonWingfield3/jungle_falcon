#pragma once

#include <algorithm>
#include <iterator>
#include <memory>

#include <memory.hpp>
#include <pipeline.hpp>
#include <register_file.hpp>

class CPU;
using CpuPtr = std::shared_ptr<CPU>;
using Breakpoint = std::pair<int, mem_addr_t>;

class CPU {
 public:
  CPU(MemoryPtr mem) : mem_(mem) {
    reg_file_ = std::make_shared<RegisterFile>(RegisterFile());
    pc_ = std::make_shared<ProgramCounter>(ProgramCounter());
    pipeline_ = std::make_shared<Pipeline>(Pipeline(reg_file_, pc_, mem_));
  }

  RegFilePtr RegFile() const { return reg_file_; }
  PcPtr PC() const { return pc_; }
  PipelinePtr PipeLine() const { return pipeline_; }

  void Reset() {
    reg_file_->Reset();
    pc_->Reset();
    pipeline_->Reset();
  }

  void ExecuteCycle(std::size_t n = 1) {
    for (int ii = 0; ii < n; ++ii) {
      if ((ii != 0) && std::any_of(bkpts_.cbegin(), bkpts_.cend(),
                                   [&](const Breakpoint& bkpt) {
                                     return bkpt.second == pc_->Reg();
                                   })) {
        VLOG(3) << "Hit breakpoint";
        break;
      }
      pipeline_->ExecuteCycle();
    }
  }

  void SetBreakpoint(mem_addr_t breakpoint_address) {
    bkpts_.push_back(
        std::make_pair(static_cast<int>(bkpts_.size()), breakpoint_address));
  }

  void DeleteBreakpoint(int bkpt_num) {
    bkpts_.erase(std::remove_if(
                     bkpts_.begin(), bkpts_.end(),
                     [&](Breakpoint& bkpt) { return bkpt.first == bkpt_num; }),
                 bkpts_.end());
  }

  const std::vector<Breakpoint>& GetBreakpoints() const { return bkpts_; }

 private:
  RegFilePtr reg_file_;
  PcPtr pc_;
  PipelinePtr pipeline_;
  MemoryPtr mem_;
  std::vector<Breakpoint> bkpts_;
};

#pragma once

#include <algorithm>
#include <iterator>
#include <memory>

#include <hazard_detection.hpp>
#include <instructions.hpp>
#include <memory.hpp>
#include <pipeline.hpp>
#include <register_file.hpp>

class CPU;
using CpuPtr = std::shared_ptr<CPU>;
using Breakpoint = std::pair<int, mem_addr_t>;

class CPU {
 public:
  CPU(MemoryPtr instr_mem, MemoryPtr data_mem);

  void Reset();
  void ExecuteCycle(std::size_t n = 1);
  void SetBreakpoint(mem_addr_t breakpoint_address);
  void DeleteBreakpoint(int bkpt_num);

  const std::vector<Breakpoint>& GetBreakpoints() const { return bkpts_; }
  RegFilePtr RegFile() const { return reg_file_; }
  PcPtr PC() const { return pc_; }
  PipelinePtr PipeLine() const { return pipeline_; }
  HazardDetectionPtr DataHazardDetector() const {
    return data_hazard_detector_;
  }
  HazardDetectionPtr ControlHazardDetector() const {
    return control_hazard_detector_;
  }

  double GetCPI() const;
  std::size_t GetCycles() const { return cycles_; }

 private:
  RegFilePtr reg_file_;
  PcPtr pc_;
  PipelinePtr pipeline_;
  HazardDetectionPtr data_hazard_detector_;
  HazardDetectionPtr control_hazard_detector_;
  MemoryPtr instr_mem_;
  MemoryPtr data_mem_;
  std::vector<Breakpoint> bkpts_;
  std::size_t cycles_ = 0;
};

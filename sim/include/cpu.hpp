#pragma once

#include <algorithm>
#include <iterator>
#include <memory>

#include <hardware_object.hpp>
#include <hazard_detection.hpp>
#include <instructions.hpp>
#include <memory.hpp>
#include <pipeline.hpp>
#include <register_file.hpp>

class CPU;
using CpuPtr = std::shared_ptr<CPU>;
using Breakpoint = std::pair<int, mem_addr_t>;

class CPU : public HardwareObject {
 public:
  CPU(MemoryPtr instr_mem, MemoryPtr data_mem);
  ~CPU() override = default;

  // Override of HardwareObject methods
  void ExecuteCycle() final;
  void Reset() final;

  // Debug
  void SetBreakpoint(mem_addr_t breakpoint_address);
  void DeleteBreakpoint(int bkpt_num);
  bool HitBreakpoint() { return at_bkpt_; }

  // Getters
  const std::vector<Breakpoint>& GetBreakpoints() const;
  RegFilePtr GetRegFile() const;
  PcPtr GetPC() const;
  PipelinePtr GetPipeline() const;
  HazardDetectionPtr GetDataHazardDetector() const;
  HazardDetectionPtr GetControlHazardDetector() const;

  // Stat functions
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
  bool at_bkpt_ = false;
  std::size_t cycles_ = 0;
};

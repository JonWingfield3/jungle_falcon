#include <cpu.hpp>

#include <algorithm>

#include <instructions.hpp>
#include <register_file.hpp>

////////////////////////////////////////////////////////////////////////////////
CPU::CPU(MemoryPtr instr_mem, MemoryPtr data_mem)
    : HardwareObject(), instr_mem_(instr_mem), data_mem_(data_mem) {
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
const std::vector<Breakpoint>& CPU::GetBreakpoints() const { return bkpts_; }

////////////////////////////////////////////////////////////////////////////////
RegFilePtr CPU::GetRegFile() const { return reg_file_; }

////////////////////////////////////////////////////////////////////////////////
PcPtr CPU::GetPC() const { return pc_; }

////////////////////////////////////////////////////////////////////////////////
PipelinePtr CPU::GetPipeline() const { return pipeline_; }

////////////////////////////////////////////////////////////////////////////////
HazardDetectionPtr CPU::GetDataHazardDetector() const {
  return data_hazard_detector_;
}

////////////////////////////////////////////////////////////////////////////////
HazardDetectionPtr CPU::GetControlHazardDetector() const {
  return control_hazard_detector_;
}

////////////////////////////////////////////////////////////////////////////////
void CPU::ExecuteCycle() {
  if (!at_bkpt_ &&
      std::any_of(bkpts_.cbegin(), bkpts_.cend(), [&](const Breakpoint& bkpt) {
        return bkpt.second == pc_->InstructionPointer();
      })) {
    VLOG(1) << "Hit breakpoint";
    at_bkpt_ = true;
  } else {
    at_bkpt_ = false;
    VLOG(1) << "##################### Start of cycle #####################";
    data_mem_->ExecuteCycle();
    instr_mem_->ExecuteCycle();
    pipeline_->ExecuteCycle();
    control_hazard_detector_->HandleHazard();
    data_hazard_detector_->HandleHazard();
    HardwareObject::ExecuteCycle();  // TODO: to exe or not exe at bkpt?
  }
}

////////////////////////////////////////////////////////////////////////////////
void CPU::Reset() {
  reg_file_->Reset();
  pc_->Reset();
  pipeline_->Reset();
  HardwareObject::Reset();
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

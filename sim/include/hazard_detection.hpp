#pragma once

#include <instructions.hpp>
#include <pipeline.hpp>

class Pipeline;
class IHazardDetectionUnit;
using HazardDetectionPtr = std::shared_ptr<IHazardDetectionUnit>;

class IHazardDetectionUnit {
 public:
  explicit IHazardDetectionUnit(PipelinePtr pipeline) : pipeline_(pipeline) {}
  virtual ~IHazardDetectionUnit() {}

  virtual void HandleHazard() = 0;

  std::size_t HazardsDetected() const { return hazards_detected_; }
  std::size_t DelayAdded() const { return delay_added_; }

 protected:
  PipelinePtr pipeline_;
  std::size_t hazards_detected_ = 0;
  std::size_t delay_added_ = 0;

  Register& GetRd(const InstructionPtr& instr) const;
  Register& GetRs1(const InstructionPtr& instr) const;
  Register& GetRs2(const InstructionPtr& instr) const;
  bool WritesToRd(const InstructionPtr& instr) const;
  bool ReadsFromRs1(const InstructionPtr& instr) const;
  bool ReadsFromRs2(const InstructionPtr& instr) const;
};

class DataHazardDetectionUnit : public IHazardDetectionUnit {
 public:
  DataHazardDetectionUnit(PipelinePtr pipeline)
      : IHazardDetectionUnit(pipeline) {}
  ~DataHazardDetectionUnit() override = default;

  void HandleHazard() final;

 private:
  // 1a type hazard forwards data from EX/MEM buf to ID/EX buf
  void Handle1aTypeHazard(InstructionPtr& decode_instr,
                          InstructionPtr& execute_instr);
  // 1b type data hazard forwards data from MEM/WB buf to ID/EX buf
  void Handle1bTypeHazard(InstructionPtr& decode_instr,
                          InstructionPtr& memory_access_instr);
  void HandleLoadHazard(InstructionPtr& execute_instr,
                        InstructionPtr& decode_instr);
};

class ControlHazardDetectionUnit : public IHazardDetectionUnit {
 public:
  ControlHazardDetectionUnit(PipelinePtr pipeline)
      : IHazardDetectionUnit(pipeline) {}
  ~ControlHazardDetectionUnit() override = default;

  void HandleHazard() final;
};

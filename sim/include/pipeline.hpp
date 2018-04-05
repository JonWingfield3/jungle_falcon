#pragma once

#include <algorithm>
#include <deque>
#include <memory>
#include <string>

#include <instruction_factory.hpp>
#include <instructions.hpp>
#include <memory.hpp>
#include <register_file.hpp>

class Pipeline;
using PipelinePtr = std::shared_ptr<Pipeline>;
using InstructionQueue = std::deque<InstructionPtr>;

class Pipeline {
 public:
  explicit Pipeline(RegFilePtr reg_file, PcPtr pc, MemoryPtr mem);

  enum Stages {
    FetchStage,
    DecodeStage,
    ExecuteStage,
    MemoryAccessStage,
    WriteBackStage,
    NumStages
  };

  void ExecuteCycle();
  void AdvanceStages();

  void Flush(std::size_t n = 5);

  const InstructionPtr& Instruction(enum Stages pipeline_stage) const;
  InstructionPtr& Instruction(enum Stages pipeline_stage);
  std::vector<std::string> InstructionNames() const;

 private:
  InstructionPtr Fetch();

  InstructionQueue instruction_queue_;
  MemoryPtr mem_;
  PcPtr pc_;
  InstructionFactory instruction_factory_;
};

class IHazardDetectionUnit;
using HazardDetectionPtr = std::shared_ptr<IHazardDetectionUnit>;

class IHazardDetectionUnit {
 public:
  explicit IHazardDetectionUnit(PipelinePtr pipeline) : pipeline_(pipeline) {}
  virtual ~IHazardDetectionUnit() {}

  virtual void HandleHazard() = 0;

 protected:
  PipelinePtr pipeline_;

  Register& GetRd(const InstructionPtr& instr);
  Register& GetRs1(const InstructionPtr& instr);
  Register& GetRs2(const InstructionPtr& instr);
  bool WritesToRd(const InstructionPtr& instr);
  bool ReadsFromRs1(const InstructionPtr& instr);
  bool ReadsFromRs2(const InstructionPtr& instr);
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
};

class ControlHazardDetectionUnit : public IHazardDetectionUnit {
 public:
  ControlHazardDetectionUnit(PipelinePtr pipeline)
      : IHazardDetectionUnit(pipeline) {}
  ~ControlHazardDetectionUnit() override = default;

  void HandleHazard() final;
};

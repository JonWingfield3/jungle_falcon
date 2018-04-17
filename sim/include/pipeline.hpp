#pragma once

#include <algorithm>
#include <deque>
#include <memory>
#include <string>

#include <hardware_object.hpp>
#include <instruction_factory.hpp>
#include <instructions.hpp>
#include <memory.hpp>
#include <register_file.hpp>

class Pipeline;
using PipelinePtr = std::shared_ptr<Pipeline>;

class Pipeline : public HardwareObject {
 public:
  explicit Pipeline(RegFilePtr reg_file, PcPtr pc, MemoryPtr instr_mem,
                    MemoryPtr data_mem);

  enum Stages {
    FetchStage,
    DecodeStage,
    ExecuteStage,
    MemoryAccessStage,
    WriteBackStage,
    NumStages
  };

  void ExecuteCycle() final;
  void Reset() final;

  void Flush(std::size_t n = WriteBackStage);
  void InsertDelay(Stages stage);

  const InstructionPtr& Instruction(enum Stages pipeline_stage) const;
  InstructionPtr& Instruction(enum Stages pipeline_stage);
  std::vector<std::string> InstructionNames() const;
  std::size_t InstructionsCompleted() const;

 private:
  using InstructionQueue = std::deque<InstructionPtr>;
  InstructionQueue instruction_queue_;

  PcPtr pc_;
  MemoryPtr instr_mem_;
  MemoryPtr data_mem_;
  InstructionFactory instruction_factory_;
  bool delay_inserted_ = false;
  std::size_t instructions_completed_ = 0;
  std::size_t branches_taken_ = 0;
  std::size_t latency_counter_ = 0;
};

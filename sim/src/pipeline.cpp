#include <pipeline.hpp>

#include <glog/logging.h>

#include <instructions.hpp>
#include <memory.hpp>

////////////////////////////////////////////////////////////////////////////////
Pipeline::Pipeline(RegFilePtr reg_file, PcPtr pc, MemoryPtr instr_mem,
                   MemoryPtr data_mem)
    : HardwareObject(),
      pc_(pc),
      instr_mem_(instr_mem),
      data_mem_(data_mem),
      instruction_factory_(reg_file, pc_, data_mem_) {
  InstructionPtr nop_instr = std::make_shared<NopInstruction>(NopInstruction());
  for (std::size_t ii = 0; ii < NumStages; ++ii) {
    instruction_queue_.push_back(nop_instr);
  }
}

////////////////////////////////////////////////////////////////////////////////
void Pipeline::Flush(std::size_t n) {
  InstructionPtr nop_instr = std::make_shared<NopInstruction>(NopInstruction());
  for (std::size_t ii = 0; ii <= n; ++ii) {
    instruction_queue_.at(ii) = nop_instr;
  }
}

////////////////////////////////////////////////////////////////////////////////
bool Pipeline::CheckHazards() const { return (latency_counter_ == 0); }

////////////////////////////////////////////////////////////////////////////////
const InstructionPtr& Pipeline::Instruction(enum Stages pipeline_stage) const {
  return instruction_queue_.at(pipeline_stage);
}

////////////////////////////////////////////////////////////////////////////////
InstructionPtr& Pipeline::Instruction(enum Stages pipeline_stage) {
  return instruction_queue_.at(pipeline_stage);
}

////////////////////////////////////////////////////////////////////////////////
void Pipeline::InsertDelay(Stages stage) {
  instruction_queue_.insert(instruction_queue_.cbegin() + stage,
                            std::make_shared<NopInstruction>());
  delay_inserted_ = true;
  VLOG(1) << "Delay inserted";
}

////////////////////////////////////////////////////////////////////////////////
void Pipeline::ExecuteCycle() {
#if (__INSTRUCTION_ACCURATE__ == 1)
  const InstructionPtr instr_ptr = Fetch();
  instr_ptr->ExecuteCycle(0);
#else

  VLOG(4) << "Latency Counter: " << latency_counter_;
  if (latency_counter_ == 0) {
    VLOG(1) << "##################### Start of cycle #####################";

    // Remove oldest instruction in pipeline
    instruction_queue_.pop_back();

    if (!delay_inserted_) {
      // Fetch instruction and prepend to instruction queue.
      const mem_addr_t instruction_pointer = pc_->InstructionPointer();
      VLOG(1) << "Program Counter: " << std::showbase << std::hex
              << instruction_pointer;
      const instr_t instr = instr_mem_->ReadWord(instruction_pointer);
      latency_counter_ = instr_mem_->GetAccessLatency();
      const InstructionPtr fetched_instr = instruction_factory_.Create(instr);
      instruction_queue_.push_front(fetched_instr);
      // Increment instruction pointer.
      pc_->ExecuteCycle();
    } else {
      delay_inserted_ = false;
    }

    for (int stage_idx = WriteBackStage; stage_idx >= FetchStage; --stage_idx) {
      const InstructionPtr& next_instr =
          Instruction(static_cast<Stages>(stage_idx));
      next_instr->ExecuteCycle(stage_idx);
      const std::size_t instr_latency = next_instr->GetCyclesForStage();
      latency_counter_ = std::max(latency_counter_, instr_latency);
    }

    if (Instruction(Pipeline::Stages::WriteBackStage)->InstructionType() !=
        InstructionTypes::NoType) {
      ++instructions_completed_;
      // VLOG(1) << write_back_stage->InstructionName();
    }
  } else {
    --latency_counter_;
  }
#endif
  HardwareObject::ExecuteCycle();
}

////////////////////////////////////////////////////////////////////////////////
void Pipeline::Reset() {
  Flush();
  instructions_completed_ = 0;
  branches_taken_ = 0;
  delay_inserted_ = false;
  HardwareObject::Reset();
}

////////////////////////////////////////////////////////////////////////////////
std::vector<std::string> Pipeline::InstructionNames() const {
  std::vector<std::string> instruction_names;
  for (const auto& instruction : instruction_queue_) {
    instruction_names.push_back(instruction->InstructionName());
  }
  return instruction_names;
}

////////////////////////////////////////////////////////////////////////////////
std::size_t Pipeline::InstructionsCompleted() const {
  return instructions_completed_;
}

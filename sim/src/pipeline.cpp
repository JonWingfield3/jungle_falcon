#include <pipeline.hpp>

#include <glog/logging.h>

#include <instructions.hpp>
#include <memory.hpp>

////////////////////////////////////////////////////////////////////////////////
Pipeline::Pipeline(RegFilePtr reg_file, PcPtr pc, MemoryPtr instr_mem,
                   MemoryPtr data_mem)
    : pc_(pc),
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
const InstructionPtr& Pipeline::Instruction(enum Stages pipeline_stage) const {
  return instruction_queue_.at(pipeline_stage);
}

////////////////////////////////////////////////////////////////////////////////
InstructionPtr& Pipeline::Instruction(enum Stages pipeline_stage) {
  return instruction_queue_.at(pipeline_stage);
}

////////////////////////////////////////////////////////////////////////////////
InstructionPtr Pipeline::Fetch() {
  VLOG(1) << "##################### Start of cycle #####################";
  VLOG(1) << "Program Counter: " << std::showbase << std::hex << pc_->Reg();
  const instr_t instr = instr_mem_->ReadWord(pc_->Reg());
  ++*pc_;
  return instruction_factory_.Create(instr);
}

////////////////////////////////////////////////////////////////////////////////
void Pipeline::InsertDelay(Stages stage) {
  instruction_queue_.insert(instruction_queue_.cbegin() + stage,
                            std::make_shared<NopInstruction>(NopInstruction()));
  delay_inserted_ = true;
  VLOG(1) << "Delay inserted";
}

////////////////////////////////////////////////////////////////////////////////
void Pipeline::ExecuteCycle() {
#if (__CYCLE_ACCURATE__ == 1)
  // Remove oldest instructionin pipeline
  instruction_queue_.pop_back();

  if (!delay_inserted_) {
    const InstructionPtr fetched_instr = Fetch();
    instruction_queue_.push_front(fetched_instr);
    VLOG(1) << "Fetch: " << fetched_instr->PreDecodedInstructionName();
  } else {
    delay_inserted_ = false;
  }

  for (int stage_idx = WriteBackStage; stage_idx > FetchStage; --stage_idx) {
    Instruction(static_cast<Stages>(stage_idx))->ExecuteCycle(stage_idx);
  }

  if (Instruction(Pipeline::Stages::WriteBackStage)->InstructionType() !=
      InstructionTypes::NoType) {
    ++instructions_completed_;
    // VLOG(1) << write_back_stage->InstructionName();
  }

#else
#if (__INSTRUCTION_ACCURATE__ == 1)
  const InstructionPtr instr_ptr = Fetch();
  instr_ptr->ExecuteCycle();
#endif
#endif
}

////////////////////////////////////////////////////////////////////////////////
std::vector<std::string> Pipeline::InstructionNames() const {
  std::vector<std::string> instruction_names;
  for (const auto& instruction : instruction_queue_) {
    instruction_names.push_back(instruction->InstructionName());
  }
  return instruction_names;
}

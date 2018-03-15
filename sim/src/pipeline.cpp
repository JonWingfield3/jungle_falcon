#include <pipeline.hpp>

#include <glog/logging.h>

#include <alu.hpp>
#include <instructions.hpp>
#include <memory.hpp>

int Pipeline::Fetch() {
  VLOG(1) << "Address: " << std::showbase << std::hex << pc_->Reg();
  VLOG(2) << "Fetching instruction from " << std::showbase << std::hex
          << pc_->Reg();
  const instr_t instr = mem_->ReadWord(pc_->Reg());
  ++*pc_;

  InstructionPtr instr_ptr = instruction_factory_.Create(instr);

  CHECK(instr_ptr != nullptr) << "Unrecognized instruction!!!";

  if (!instruction_queue_.empty()) {
    instruction_queue_.pop_back();
  }
  instruction_queue_.push_front(instr_ptr);

  return 0;
}

void Pipeline::ExecuteCycle() {
#if defined(__INSTRUCTION_ACCURATE__)
  Fetch();
  InstructionPtr instr_ptr = instruction_queue_.front();
  instr_ptr->ExecuteCycle();
#endif
}

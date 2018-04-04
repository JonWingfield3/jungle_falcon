#include <pipeline.hpp>

#include <glog/logging.h>

#include <b_type_instructions.hpp>
#include <i_type_instructions.hpp>
#include <instructions.hpp>
#include <j_type_instructions.hpp>
#include <memory.hpp>
#include <r_type_instructions.hpp>
#include <s_type_instructions.hpp>
#include <u_type_instructions.hpp>

////////////////////////////////////////////////////////////////////////////////
InstructionPtr Pipeline::Fetch() {
  VLOG(1) << "Address: " << std::showbase << std::hex << pc_->Reg();
  VLOG(2) << "Fetching instruction from " << std::showbase << std::hex
          << pc_->Reg();
  const instr_t instr = mem_->ReadWord(pc_->Reg());
  ++*pc_;

  InstructionPtr instr_ptr = instruction_factory_.Create(instr);

  CHECK(instr_ptr != nullptr) << "Unrecognized instruction!!!";
  return instr_ptr;
}

////////////////////////////////////////////////////////////////////////////////
void Pipeline::ExecuteCycle() {
#if (__CYCLE_ACCURATE__ == 1)
  instruction_queue_.pop_back();

  const InstructionPtr fetched_instr = Fetch();
  instruction_queue_.push_front(fetched_instr);

  const auto& write_back_stage = Instruction(WriteBackStage);
  write_back_stage->WriteBack();

  const auto& memory_access_stage = Instruction(MemoryAccessStage);
  memory_access_stage->MemoryAccess();

  const auto& execute_stage = Instruction(ExecuteStage);
  execute_stage->Execute();

  const auto decode_stage = Instruction(DecodeStage);
  decode_stage->Decode();

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

////////////////////////////////////////////////////////////////////////////////
bool IHazardDetectionUnit::WritesToRd(const InstructionPtr& instr) {
  return (instr->IsIType() || instr->IsJType() || instr->IsRType() ||
          instr->IsUType());
}

////////////////////////////////////////////////////////////////////////////////
bool IHazardDetectionUnit::ReadsFromRs1(const InstructionPtr& instr) {
  return (instr->IsBType() || instr->IsIType() || instr->IsRType() ||
          instr->IsSType());
}

////////////////////////////////////////////////////////////////////////////////
bool IHazardDetectionUnit::ReadsFromRs2(const InstructionPtr& instr) {
  return (instr->IsBType() || instr->IsRType() || instr->IsSType());
}

////////////////////////////////////////////////////////////////////////////////
Register& IHazardDetectionUnit::GetRd(const InstructionPtr& instr) {
  VLOG(4) << "Hazard detector GetRd: instr type = "
          << static_cast<int>(instr->InstructionType());
  if (instr->IsIType()) {
    return reinterpret_cast<ITypeInstructionInterface*>(instr.get())->Rd();
  } else if (instr->IsJType()) {
    return reinterpret_cast<JalInstruction*>(instr.get())->Rd();
  } else if (instr->IsRType()) {
    return reinterpret_cast<RTypeInstructionInterface*>(instr.get())->Rd();
  } else if (instr->IsUType()) {
    return reinterpret_cast<UTypeInstructionInterface*>(instr.get())->Rd();
  } else {
    CHECK(false) << "Unrecognized type!"
                 << static_cast<int>(instr->InstructionType());
  }
}

////////////////////////////////////////////////////////////////////////////////
Register& IHazardDetectionUnit::GetRs1(const InstructionPtr& instr) {
  VLOG(4) << "Hazard detector GetRs1: instr type = "
          << static_cast<int>(instr->InstructionType());
  if (instr->IsBType()) {
    return reinterpret_cast<BTypeInstructionInterface*>(instr.get())->Rs1();
  } else if (instr->IsIType()) {
    return reinterpret_cast<ITypeInstructionInterface*>(instr.get())->Rs1();
  } else if (instr->IsRType()) {
    return reinterpret_cast<RTypeInstructionInterface*>(instr.get())->Rs1();
  } else if (instr->IsSType()) {
    return reinterpret_cast<STypeInstructionInterface*>(instr.get())->Rs1();
  } else {
    CHECK(false) << "Unrecognized type!"
                 << static_cast<int>(instr->InstructionType());
  }
}

////////////////////////////////////////////////////////////////////////////////
Register& IHazardDetectionUnit::GetRs2(const InstructionPtr& instr) {
  if (instr->IsBType()) {
    return reinterpret_cast<BTypeInstructionInterface*>(instr.get())->Rs2();
  } else if (instr->IsRType()) {
    return reinterpret_cast<RTypeInstructionInterface*>(instr.get())->Rs2();
  } else if (instr->IsSType()) {
    return reinterpret_cast<STypeInstructionInterface*>(instr.get())->Rs2();
  } else {
    CHECK(false) << "Unrecognized type!"
                 << static_cast<int>(instr->InstructionType());
  }
}

////////////////////////////////////////////////////////////////////////////////
void DataHazardDetectionUnit::HandleHazard() {
  InstructionPtr decode_instr =
      pipeline_->Instruction(Pipeline::Stages::DecodeStage);
  InstructionPtr execute_instr =
      pipeline_->Instruction(Pipeline::Stages::ExecuteStage);
  InstructionPtr mem_access_instr =
      pipeline_->Instruction(Pipeline::Stages::MemoryAccessStage);

  Handle1bTypeHazard(decode_instr, mem_access_instr);
  Handle1aTypeHazard(decode_instr, execute_instr);
}

////////////////////////////////////////////////////////////////////////////////
void DataHazardDetectionUnit::Handle1aTypeHazard(
    InstructionPtr& decode_instr, InstructionPtr& execute_instr) {
  if (ReadsFromRs1(decode_instr) && WritesToRd(execute_instr)) {
    // possibility of data hazard
    VLOG(3) << "Potential 1a hazard with rs1, instructions: "
            << decode_instr->InstructionName() << " and "
            << execute_instr->InstructionName();
    auto& execute_rd = GetRd(execute_instr);
    auto& decode_rs1 = GetRs1(decode_instr);
    if (execute_rd.Number() == decode_rs1.Number() &&
        execute_rd.Number() != 0) {
      decode_rs1.Data() = execute_rd.Data();
      VLOG(2) << "-1a rs1- FORWARDING: " << execute_rd;
    }
  }
  if (ReadsFromRs2(decode_instr) && WritesToRd(execute_instr)) {
    // possibility of data hazard
    VLOG(3) << "Potential 1a hazard with rs2, instructions: "
            << decode_instr->InstructionName() << " and "
            << execute_instr->InstructionName();
    auto& execute_rd = GetRd(execute_instr);
    auto& decode_rs2 = GetRs2(decode_instr);
    if (execute_rd.Number() == decode_rs2.Number() &&
        execute_rd.Number() != 0) {
      decode_rs2.Data() = execute_rd.Data();
      VLOG(2) << "-1a rs2- FORWARDING: " << execute_rd;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
void DataHazardDetectionUnit::Handle1bTypeHazard(
    InstructionPtr& decode_instr, InstructionPtr& memory_access_instr) {
  if (ReadsFromRs1(decode_instr) && WritesToRd(memory_access_instr)) {
    // possibility of data hazard
    VLOG(3) << "Potential 1b hazard with rs1, instructions: "
            << decode_instr->InstructionName() << " and "
            << memory_access_instr->InstructionName();
    auto& memory_access_rd = GetRd(memory_access_instr);
    auto& decode_rs1 = GetRs1(decode_instr);
    if (memory_access_rd.Number() == decode_rs1.Number() &&
        memory_access_rd.Number() != 0) {
      decode_rs1.Data() = memory_access_rd.Data();
      VLOG(2) << "-1b rs1- FORWARDING: " << memory_access_rd;
    }
  }
  if (ReadsFromRs2(decode_instr) && WritesToRd(memory_access_instr)) {
    // possibility of data hazard
    VLOG(3) << "Potential 1b hazard with rs2, instructions: "
            << decode_instr->InstructionName() << " and "
            << memory_access_instr->InstructionName();
    auto& memory_access_rd = GetRd(memory_access_instr);
    auto& decode_rs2 = GetRs2(decode_instr);
    if (memory_access_rd.Number() == decode_rs2.Number() &&
        memory_access_rd.Number() != 0) {
      decode_rs2.Data() = memory_access_rd.Data();
      VLOG(2) << "-1b rs2- FORWARDING: " << memory_access_rd;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
void ControlHazardDetectionUnit::HandleHazard() {}

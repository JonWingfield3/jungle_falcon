#include <hazard_detection.hpp>

#include <b_type_instructions.hpp>
#include <i_type_instructions.hpp>
#include <instructions.hpp>
#include <j_type_instructions.hpp>
#include <pipeline.hpp>
#include <r_type_instructions.hpp>
#include <s_type_instructions.hpp>
#include <u_type_instructions.hpp>

////////////////////////////////////////////////////////////////////////////////
bool IHazardDetectionUnit::WritesToRd(const InstructionPtr& instr) const {
  return (instr->IsIType() || instr->IsJType() || instr->IsRType() ||
          instr->IsUType());
}

////////////////////////////////////////////////////////////////////////////////
bool IHazardDetectionUnit::ReadsFromRs1(const InstructionPtr& instr) const {
  return (instr->IsBType() || instr->IsIType() || instr->IsRType() ||
          instr->IsSType());
}

////////////////////////////////////////////////////////////////////////////////
bool IHazardDetectionUnit::ReadsFromRs2(const InstructionPtr& instr) const {
  return (instr->IsBType() || instr->IsRType() || instr->IsSType());
}

////////////////////////////////////////////////////////////////////////////////
Register& IHazardDetectionUnit::GetRd(const InstructionPtr& instr) const {
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

  // won't ever reach here but include this to avoid compiler warnings
  return reinterpret_cast<UTypeInstructionInterface*>(instr.get())->Rd();
}

////////////////////////////////////////////////////////////////////////////////
Register& IHazardDetectionUnit::GetRs1(const InstructionPtr& instr) const {
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

  // won't ever reach here but include this to avoid compiler warnings
  return reinterpret_cast<STypeInstructionInterface*>(instr.get())->Rs1();
}

////////////////////////////////////////////////////////////////////////////////
Register& IHazardDetectionUnit::GetRs2(const InstructionPtr& instr) const {
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

  // won't ever reach here but include this to avoid compiler warnings
  return reinterpret_cast<STypeInstructionInterface*>(instr.get())->Rs2();
}

////////////////////////////////////////////////////////////////////////////////
void DataHazardDetectionUnit::HandleHazard() {
  InstructionPtr decode_instr =
      pipeline_->Instruction(Pipeline::Stages::DecodeStage);
  InstructionPtr execute_instr =
      pipeline_->Instruction(Pipeline::Stages::ExecuteStage);
  InstructionPtr mem_access_instr =
      pipeline_->Instruction(Pipeline::Stages::MemoryAccessStage);

  HandleLoadHazard(decode_instr, execute_instr);
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
      ++hazards_detected_;
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
      ++hazards_detected_;
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
      ++hazards_detected_;
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
      ++hazards_detected_;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
void DataHazardDetectionUnit::HandleLoadHazard(InstructionPtr& decode_instr,
                                               InstructionPtr& execute_instr) {
  if (execute_instr->GetOpCode() != OpCode::Lx) {
    return;
  }

  if (ReadsFromRs1(decode_instr) && WritesToRd(execute_instr)) {
    // possibility of data hazard
    VLOG(3) << "Potential load hazard with rs1, instructions: "
            << execute_instr->InstructionName() << " and "
            << decode_instr->InstructionName();
    auto& decode_rs1 = GetRs1(decode_instr);
    auto& execute_rd = GetRd(execute_instr);
    if (execute_rd.Number() == decode_rs1.Number() &&
        execute_rd.Number() != 0) {
      VLOG(1) << "Inserting delay (Rs1)";
      pipeline_->InsertDelay(Pipeline::Stages::ExecuteStage);
      ++hazards_detected_;
      ++delay_added_;
    }
  }

  if (ReadsFromRs2(decode_instr) && WritesToRd(execute_instr)) {
    // possibility of data hazard
    VLOG(3) << "Potential load hazard with rs2, instructions: "
            << execute_instr->InstructionName() << " and "
            << decode_instr->InstructionName();
    auto& decode_rs2 = GetRs2(decode_instr);
    auto& execute_rd = GetRd(execute_instr);
    if (execute_rd.Number() == decode_rs2.Number() &&
        execute_rd.Number() != 0) {
      VLOG(1) << "Inserting delay (Rs2)";
      pipeline_->InsertDelay(Pipeline::Stages::ExecuteStage);
      ++hazards_detected_;
      ++delay_added_;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
void ControlHazardDetectionUnit::HandleHazard() {
  InstructionPtr instr =
      pipeline_->Instruction(Pipeline::Stages::MemoryAccessStage);

  if ((instr->IsBType() &&
       reinterpret_cast<BTypeInstructionInterface*>(instr.get())
           ->WillBranch()) ||
      instr->IsJType() ||
      !strcmp(instr->PreDecodedInstructionName().c_str(), "jalr")) {
    VLOG(2) << "Detected a branch! Flushing pipeline";
    pipeline_->Flush(Pipeline::Stages::ExecuteStage);
    ++hazards_detected_;
    delay_added_ += 3;
  }
}

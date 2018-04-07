#include <instructions.hpp>

#include <pipeline.hpp>

////////////////////////////////////////////////////////////////////////////////
void InstructionInterface::ExecuteCycle(int stage) {
  switch (stage) {
    case Pipeline::Stages::DecodeStage:
      Decode();
      break;
    case Pipeline::Stages::ExecuteStage:
      Execute();
      break;
    case Pipeline::Stages::MemoryAccessStage:
      MemoryAccess();
      break;
    case Pipeline::Stages::WriteBackStage:
      WriteBack();
      break;
  }
}

////////////////////////////////////////////////////////////////////////////////
void InstructionInterface::Decode() {
  SetInstructionName();
  VLOG(2) << "Decode: "
          << instruction_;  // << "\tRegisters: " << RegistersString();
}

////////////////////////////////////////////////////////////////////////////////
void InstructionInterface::Execute() {
  VLOG(2) << "Execute: " << instruction_;
  //          << "\tRegisters: " << RegistersString();
}

////////////////////////////////////////////////////////////////////////////////
void InstructionInterface::MemoryAccess() {
  VLOG(2) << "Memory Access: " << instruction_;
  //          << "\tRegisters: " << RegistersString();
}

////////////////////////////////////////////////////////////////////////////////
void InstructionInterface::WriteBack() {
  VLOG(2) << "Write Back: " << instruction_;
  //          << "\tRegisters: " << RegistersString();
}

////////////////////////////////////////////////////////////////////////////////
InstructionTypes InstructionInterface::InstructionType() const {
  return instruction_type_;
}

////////////////////////////////////////////////////////////////////////////////
const std::string& InstructionInterface::InstructionName() const {
  return instruction_;
}

////////////////////////////////////////////////////////////////////////////////
bool InstructionInterface::IsBType() const {
  return instruction_type_ == InstructionTypes::BType;
}

////////////////////////////////////////////////////////////////////////////////
bool InstructionInterface::IsIType() const {
  return instruction_type_ == InstructionTypes::IType;
}

////////////////////////////////////////////////////////////////////////////////
bool InstructionInterface::IsJType() const {
  return instruction_type_ == InstructionTypes::Jtype;
}

////////////////////////////////////////////////////////////////////////////////
bool InstructionInterface::IsSType() const {
  return instruction_type_ == InstructionTypes::SType;
}

////////////////////////////////////////////////////////////////////////////////
bool InstructionInterface::IsRType() const {
  return instruction_type_ == InstructionTypes::RType;
}

////////////////////////////////////////////////////////////////////////////////
bool InstructionInterface::IsUType() const {
  return instruction_type_ == InstructionTypes::UType;
}

////////////////////////////////////////////////////////////////////////////////
void NopInstruction::Decode() {
  VLOG(2) << "Decode: NOP";
  SetInstructionName();
}

////////////////////////////////////////////////////////////////////////////////
void NopInstruction::Execute() { VLOG(2) << "Execute: NOP"; }

////////////////////////////////////////////////////////////////////////////////
void NopInstruction::MemoryAccess() { VLOG(2) << "Memory Access: NOP"; }

////////////////////////////////////////////////////////////////////////////////
void NopInstruction::WriteBack() { VLOG(2) << "Write Back: NOP"; }

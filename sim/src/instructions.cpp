#include <instructions.hpp>

////////////////////////////////////////////////////////////////////////////////
void InstructionInterface::ExecuteCycle() {
  Decode();
  Execute();
  MemoryAccess();
  WriteBack();
}

////////////////////////////////////////////////////////////////////////////////
void InstructionInterface::Decode() {
  SetInstructionName();
  VLOG(1) << "Decode: " << instruction_ << "\tRegisters: " << RegistersString();
}

////////////////////////////////////////////////////////////////////////////////
void InstructionInterface::Execute() {
  VLOG(1) << "Execute: " << instruction_
          << "\tRegisters: " << RegistersString();
}

////////////////////////////////////////////////////////////////////////////////
void InstructionInterface::MemoryAccess() {
  VLOG(1) << "Memory Access: " << instruction_
          << "\tRegisters: " << RegistersString();
}

////////////////////////////////////////////////////////////////////////////////
void InstructionInterface::WriteBack() {
  VLOG(1) << "Write Back: " << instruction_
          << "\tRegisters: " << RegistersString();
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
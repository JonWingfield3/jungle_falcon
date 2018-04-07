#include <u_type_instructions.hpp>

////////////////////////////////////////////////////////////////////////////////
UTypeInstructionInterface::UTypeInstructionInterface(instr_t instr,
                                                     RegFilePtr reg_file)
    : InstructionInterface(instr), reg_file_(reg_file) {
  name_ = "U Type Instruction";
  instruction_type_ = InstructionTypes::UType;
}

////////////////////////////////////////////////////////////////////////////////
void UTypeInstructionInterface::Decode() {
  UTypeInstructionFormat u_type_format;
  u_type_format.word = instr_;

  const int rd_num = u_type_format.rd;
  Rd_ = std::make_shared<Register>(Register(rd_num));
  reg_file_->Read(*Rd_);

  imm_ = static_cast<uint32_t>(u_type_format.imm31_12);

  InstructionInterface::Decode();
}

////////////////////////////////////////////////////////////////////////////////
void UTypeInstructionInterface::Execute() {
  Rd_->Data() = (imm_ << 12);
  InstructionInterface::Execute();
}

////////////////////////////////////////////////////////////////////////////////
void UTypeInstructionInterface::WriteBack() {
  reg_file_->Write(*Rd_);
  InstructionInterface::WriteBack();
}

////////////////////////////////////////////////////////////////////////////////
void UTypeInstructionInterface::SetInstructionName() {
  std::stringstream instruction_stream;
  instruction_stream << name_ << " x" << Rd_->Number() << ", " << imm_;
  instruction_ = instruction_stream.str();
}

////////////////////////////////////////////////////////////////////////////////
std::string UTypeInstructionInterface::RegistersString() {
  std::stringstream reg_str;
  reg_str << "rd: " << Rd();
  return reg_str.str();
}

////////////////////////////////////////////////////////////////////////////////
LuiInstruction::LuiInstruction(instr_t instr, RegFilePtr reg_file)
    : UTypeInstructionInterface(instr, reg_file) {
  name_ = "lui";
}

////////////////////////////////////////////////////////////////////////////////
void LuiInstruction::Execute() {
  Rd_->Data() = imm_;
  UTypeInstructionInterface::Execute();
}

////////////////////////////////////////////////////////////////////////////////
AuipcInstruction::AuipcInstruction(instr_t instr, RegFilePtr reg_file, PcPtr pc)
    : UTypeInstructionInterface(instr, reg_file), pc_(pc) {
  name_ = "auipc";
}

////////////////////////////////////////////////////////////////////////////////
void AuipcInstruction::Execute() {
  reg_data_t pc_offset = static_cast<reg_data_t>(static_cast<int>(pc_->Reg()) +
                                                 static_cast<int>(imm_));
  VLOG(3) << "Execute: computed pc offset address " << pc_offset;
  Rd_->Data() = pc_offset;
  UTypeInstructionInterface::Execute();
}

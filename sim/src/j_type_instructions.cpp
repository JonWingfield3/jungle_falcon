#include <j_type_instructions.hpp>
#include <pipeline.hpp>

////////////////////////////////////////////////////////////////////////////////
JalInstruction::JalInstruction(instr_t instr, RegFilePtr reg_file, PcPtr pc)
    : InstructionInterface(instr), reg_file_(reg_file), pc_(pc) {
  name_ = "jal";
  instruction_type_ = InstructionTypes::Jtype;
}

////////////////////////////////////////////////////////////////////////////////
void JalInstruction::Decode() {
  JTypeInstructionFormat j_type_format;
  j_type_format.word = instr_;

  const int rd_num = j_type_format.rd;
  Rd_ = std::make_shared<Register>(Register(rd_num));
  reg_file_->Read(*Rd_);

  const imm_t imm_upper_11 = (j_type_format.imm20 ? -1 : 0) & ~(0xfffff);
  imm_ = static_cast<imm_t>(imm_upper_11 | (j_type_format.imm20 << 20) |
                            (j_type_format.imm19_12 << 12) |
                            (j_type_format.imm11 << 11) |
                            (j_type_format.imm10_1 << 1));

  InstructionInterface::Decode();
}

////////////////////////////////////////////////////////////////////////////////
void JalInstruction::Execute() {
#if (__INSTRUCTION_ACCURATE__ == 1)
  const int pipeline_offset = sizeof(instr_t);
#else
  const int pipeline_offset =
      (Pipeline::Stages::DecodeStage - Pipeline::Stages::FetchStage + 1) *
      sizeof(instr_t);

#endif
  Rd_->Data() = pc_->InstructionPointer() + pipeline_offset;
  InstructionInterface::Execute();
}

////////////////////////////////////////////////////////////////////////////////
void JalInstruction::MemoryAccess() {
  pc_->BranchOffset(imm_);
  InstructionInterface::MemoryAccess();
}

////////////////////////////////////////////////////////////////////////////////
void JalInstruction::WriteBack() {
  reg_file_->Write(*Rd_);
  InstructionInterface::WriteBack();
}

////////////////////////////////////////////////////////////////////////////////
void JalInstruction::SetInstructionName() {
  std::stringstream instruction_stream;
  instruction_stream << name_ << " x" << Rd_->Number() << ", " << imm_;
  instruction_ = instruction_stream.str();
}

////////////////////////////////////////////////////////////////////////////////
std::string JalInstruction::RegistersString() {
  std::stringstream reg_str;
  reg_str << "rd: " << Rd();
  return reg_str.str();
}

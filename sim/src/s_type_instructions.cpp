#include <s_type_instructions.hpp>

////////////////////////////////////////////////////////////////////////////////
STypeInstructionInterface::STypeInstructionInterface(instr_t instr,
                                                     RegFilePtr reg_file,
                                                     MemoryPtr mem)
    : InstructionInterface(instr), reg_file_(reg_file), mem_(mem) {
  name_ = "S Type Instruction";
  instruction_type_ = InstructionTypes::SType;
}

////////////////////////////////////////////////////////////////////////////////
void STypeInstructionInterface::Decode() {
  STypeInstructionFormat s_type_format;
  s_type_format.word = instr_;

  const int rs1_num = s_type_format.rs1;
  Rs1_ = std::make_shared<Register>(Register(rs1_num));
  reg_file_->Read(*Rs1_);

  const int rs2_num = s_type_format.rs2;
  Rs2_ = std::make_shared<Register>(Register(rs2_num));
  reg_file_->Read(*Rs2_);

  imm_t imm_upper_20 = (((s_type_format.imm11_5 << 5) & (1 << 11)) ? -1 : 0);
  imm_upper_20 &= ~(0xfff);
  imm_ = static_cast<imm_t>(imm_upper_20 | (s_type_format.imm11_5 << 5) |
                            (s_type_format.imm4_0));

  InstructionInterface::Decode();
}

////////////////////////////////////////////////////////////////////////////////
void STypeInstructionInterface::Execute() {
  store_address_ = Rs1_->Data() + imm_;
  VLOG(3) << "Execute: store address calculated as " << std::hex
          << std::showbase << store_address_;
  InstructionInterface::Execute();
}

////////////////////////////////////////////////////////////////////////////////
void STypeInstructionInterface::MemoryAccess() {
  VLOG(3) << "MemoryAccess: writing" << Rs2_ << " to mem address"
          << store_address_;
  cycles_for_stage_ = mem_->GetAccessLatency();
  InstructionInterface::MemoryAccess();
}

////////////////////////////////////////////////////////////////////////////////
void STypeInstructionInterface::WriteBack() {
  VLOG(3) << "WriteBack stage: Doing nothing";
  cycles_for_stage_ = 0;
  InstructionInterface::WriteBack();
}

////////////////////////////////////////////////////////////////////////////////
void STypeInstructionInterface::SetInstructionName() {
  std::stringstream instruction_stream;
  instruction_stream << name_ << " x" << Rs2_->Number() << ", "
                     << static_cast<int>(imm_) << "(x" << Rs1_->Number() << ")";
  instruction_ = instruction_stream.str();
}

////////////////////////////////////////////////////////////////////////////////
std::string STypeInstructionInterface::RegistersString() {
  std::stringstream reg_str;
  reg_str << ", rs1: " << Rs1() << ", rs2: " << Rs2();
  return reg_str.str();
}

////////////////////////////////////////////////////////////////////////////////
SbInstruction::SbInstruction(instr_t instr, RegFilePtr reg_file, MemoryPtr mem)
    : STypeInstructionInterface(instr, reg_file, mem) {
  name_ = "sb";
}

////////////////////////////////////////////////////////////////////////////////
void SbInstruction::MemoryAccess() {
  const uint8_t store_byte = static_cast<uint8_t>(Rs2_->Data());
  mem_->WriteByte(store_address_, store_byte);
  STypeInstructionInterface::MemoryAccess();
}

////////////////////////////////////////////////////////////////////////////////
ShInstruction::ShInstruction(instr_t instr, RegFilePtr reg_file, MemoryPtr mem)
    : STypeInstructionInterface(instr, reg_file, mem) {
  name_ = "sh";
}

////////////////////////////////////////////////////////////////////////////////
void ShInstruction::MemoryAccess() {
  const uint16_t store_halfword = static_cast<uint16_t>(Rs2_->Data());
  mem_->WriteHalfWord(store_address_, store_halfword);
  STypeInstructionInterface::MemoryAccess();
}

////////////////////////////////////////////////////////////////////////////////
SwInstruction::SwInstruction(instr_t instr, RegFilePtr reg_file, MemoryPtr mem)
    : STypeInstructionInterface(instr, reg_file, mem) {
  name_ = "sw";
}

////////////////////////////////////////////////////////////////////////////////
void SwInstruction::MemoryAccess() {
  const uint32_t store_word = static_cast<uint32_t>(Rs2_->Data());
  mem_->WriteWord(store_address_, store_word);
  STypeInstructionInterface::MemoryAccess();
}

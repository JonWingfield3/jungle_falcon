#include <b_type_instructions.hpp>

////////////////////////////////////////////////////////////////////////////////
BTypeInstructionInterface::BTypeInstructionInterface(instr_t instr,
                                                     RegFilePtr reg_file,
                                                     PcPtr& pc)
    : InstructionInterface(instr), reg_file_(reg_file), pc_(pc) {
  name_ = "B Type Instruction";
  instruction_type_ = InstructionTypes::BType;
}

////////////////////////////////////////////////////////////////////////////////
void BTypeInstructionInterface::Decode() {
  BTypeInstructionFormat b_type_format;
  b_type_format.word = instr_;

  const int rs1_num = b_type_format.rs1;
  Rs1_ = std::make_shared<Register>(Register(rs1_num));
  reg_file_->Read(*Rs1_);

  const int rs2_num = b_type_format.rs2;
  Rs2_ = std::make_shared<Register>(Register(rs2_num));
  reg_file_->Read(*Rs2_);

  const imm_t imm_upper_20 =
      static_cast<imm_t>((b_type_format.imm12 ? -1 : 0)) & ~(0x1fff);
  imm_ = static_cast<int>(
      imm_upper_20 | (b_type_format.imm12 << 12) | (b_type_format.imm11 << 11) |
      (b_type_format.imm10_5 << 5) | (b_type_format.imm4_1 << 1));

  InstructionInterface::Decode();
}

////////////////////////////////////////////////////////////////////////////////
void BTypeInstructionInterface::Execute() {
  VLOG(3) << "Taking branch: " << branch_;
  InstructionInterface::Execute();
}

////////////////////////////////////////////////////////////////////////////////
void BTypeInstructionInterface::MemoryAccess() {
  if (branch_) {
    VLOG(1) << "PC Pre Branch: " << std::hex << std::showbase << pc_->Reg();
    pc_->BranchOffset(imm_);
    VLOG(1) << "Branched to address: " << std::hex << std::showbase
            << pc_->Reg();
  }
  InstructionInterface::MemoryAccess();
}

////////////////////////////////////////////////////////////////////////////////
void BTypeInstructionInterface::WriteBack() {
  InstructionInterface::WriteBack();
}

////////////////////////////////////////////////////////////////////////////////
void BTypeInstructionInterface::SetInstructionName() {
  std::stringstream instruction_stream;
  instruction_stream << name_ << " x" << Rs1_->Number() << ", x"
                     << Rs2_->Number() << ", " << imm_;
  instruction_ = instruction_stream.str();
}

////////////////////////////////////////////////////////////////////////////////
std::string BTypeInstructionInterface::RegistersString() {
  std::stringstream reg_str;
  reg_str << "rs1: " << Rs1() << ", rs2: " << Rs2();
  return reg_str.str();
}

////////////////////////////////////////////////////////////////////////////////
BeqInstruction::BeqInstruction(instr_t instr, RegFilePtr reg_file, PcPtr pc)
    : BTypeInstructionInterface(instr, reg_file, pc) {
  name_ = "beq";
}

////////////////////////////////////////////////////////////////////////////////
void BeqInstruction::Execute() {
  branch_ = (Rs1_->Data() == Rs2_->Data());
  BTypeInstructionInterface::Execute();
}

////////////////////////////////////////////////////////////////////////////////
BneInstruction::BneInstruction(instr_t instr, RegFilePtr reg_file, PcPtr pc)
    : BTypeInstructionInterface(instr, reg_file, pc) {
  name_ = "bne";
}

////////////////////////////////////////////////////////////////////////////////
void BneInstruction::Execute() {
  branch_ = (Rs1_->Data() != Rs2_->Data());
  BTypeInstructionInterface::Execute();
}

////////////////////////////////////////////////////////////////////////////////
BltInstruction::BltInstruction(instr_t instr, RegFilePtr reg_file, PcPtr pc)
    : BTypeInstructionInterface(instr, reg_file, pc) {
  name_ = "blt";
}

////////////////////////////////////////////////////////////////////////////////
void BltInstruction::Execute() {
  branch_ = (static_cast<signed_reg_data_t>(Rs1_->Data()) <
             static_cast<signed_reg_data_t>(Rs2_->Data()));
  BTypeInstructionInterface::Execute();
}

////////////////////////////////////////////////////////////////////////////////
BgeInstruction::BgeInstruction(instr_t instr, RegFilePtr reg_file, PcPtr pc)
    : BTypeInstructionInterface(instr, reg_file, pc) {
  name_ = "bge";
}

////////////////////////////////////////////////////////////////////////////////
void BgeInstruction::Execute() {
  branch_ = (static_cast<signed_reg_data_t>(Rs1_->Data()) >=
             static_cast<signed_reg_data_t>(Rs2_->Data()));
  BTypeInstructionInterface::Execute();
}

////////////////////////////////////////////////////////////////////////////////
BltuInstruction::BltuInstruction(instr_t instr, RegFilePtr reg_file, PcPtr pc)
    : BTypeInstructionInterface(instr, reg_file, pc) {
  name_ = "bltu";
}

////////////////////////////////////////////////////////////////////////////////
void BltuInstruction::Execute() {
  branch_ = (Rs1_->Data() < Rs2_->Data());
  BTypeInstructionInterface::Execute();
}

////////////////////////////////////////////////////////////////////////////////
BgeuInstruction::BgeuInstruction(instr_t instr, RegFilePtr reg_file, PcPtr pc)
    : BTypeInstructionInterface(instr, reg_file, pc) {
  name_ = "bgeu";
}

////////////////////////////////////////////////////////////////////////////////
void BgeuInstruction::Execute() {
  branch_ = (Rs1_->Data() >= Rs2_->Data());
  BTypeInstructionInterface::Execute();
}

#include <r_type_instructions.hpp>

////////////////////////////////////////////////////////////////////////////////
RTypeInstructionInterface::RTypeInstructionInterface(instr_t instr,
                                                     RegFilePtr reg_file)
    : InstructionInterface(instr), reg_file_(reg_file) {
  name_ = "R Type Instruction";
  instruction_type_ = InstructionTypes::RType;
}

////////////////////////////////////////////////////////////////////////////////
void RTypeInstructionInterface::Decode() {
  RTypeInstructionFormat r_type_format;
  r_type_format.word = instr_;

  const int rd_num = r_type_format.rd;
  Rd_ = std::make_shared<Register>(Register(rd_num));
  reg_file_->Read(*Rd_);

  const int rs1_num = r_type_format.rs1;
  Rs1_ = std::make_shared<Register>(Register(rs1_num));
  reg_file_->Read(*Rs1_);

  const int rs2_num = r_type_format.rs2;
  Rs2_ = std::make_shared<Register>(Register(rs2_num));
  reg_file_->Read(*Rs2_);

  InstructionInterface::Decode();
}

////////////////////////////////////////////////////////////////////////////////
void RTypeInstructionInterface::Execute() { InstructionInterface::Execute(); }

////////////////////////////////////////////////////////////////////////////////
void RTypeInstructionInterface::MemoryAccess() {
  InstructionInterface::MemoryAccess();
}

////////////////////////////////////////////////////////////////////////////////
void RTypeInstructionInterface::WriteBack() {
  reg_file_->Write(*Rd_);
  InstructionInterface::WriteBack();
}

////////////////////////////////////////////////////////////////////////////////
void RTypeInstructionInterface::SetInstructionName() {
  std::stringstream instruction_stream;
  instruction_stream << name_ << " x" << static_cast<int>(Rd_->Number())
                     << ", x" << static_cast<int>(Rs1_->Number()) << ", x"
                     << static_cast<int>(Rs2_->Number());
  instruction_ = instruction_stream.str();
}

////////////////////////////////////////////////////////////////////////////////
std::string RTypeInstructionInterface::RegistersString() {
  std::stringstream reg_str;
  reg_str << "rd: " << Rd() << ", rs1: " << Rs1() << ", rs2: " << Rs2();
  return reg_str.str();
}

///
/// Specific R Type instructions follow
///

////////////////////////////////////////////////////////////////////////////////
SxxiInstructionInterface::SxxiInstructionInterface(instr_t instr,
                                                   RegFilePtr reg_file)
    : RTypeInstructionInterface(instr, reg_file) {
  name_ = "slli";
}

////////////////////////////////////////////////////////////////////////////////
void SxxiInstructionInterface::Execute() {
  shamt_ = Rs2_->Number();
  VLOG(3) << "Executed: " << *Rd_ << " = " << name_ << "{" << Rs1_ << ", "
          << *Rs2_ << "}";
}

////////////////////////////////////////////////////////////////////////////////
void SxxiInstructionInterface::SetInstructionName() {
  std::stringstream instruction_stream;
  instruction_stream << name_ << " "
                     << "x" << Rd_->Number() << ", x" << Rs1_->Number() << ", "
                     << Rs2_->Number();
  instruction_ = instruction_stream.str();
}

////////////////////////////////////////////////////////////////////////////////
SlliInstruction::SlliInstruction(instr_t instr, RegFilePtr reg_file)
    : SxxiInstructionInterface(instr, reg_file) {
  name_ = "slli";
}

////////////////////////////////////////////////////////////////////////////////
void SlliInstruction::Execute() {
  SxxiInstructionInterface::Execute();
  Rd_->Data() = Rs1_->Data() << shamt_;
}

////////////////////////////////////////////////////////////////////////////////
SrliInstruction::SrliInstruction(instr_t instr, RegFilePtr reg_file)
    : SxxiInstructionInterface(instr, reg_file) {
  name_ = "srli";
}

////////////////////////////////////////////////////////////////////////////////
void SrliInstruction::Execute() {
  SxxiInstructionInterface::Execute();
  Rd_->Data() = Rs1_->Data() >> shamt_;
}

////////////////////////////////////////////////////////////////////////////////
SraiInstruction::SraiInstruction(instr_t instr, RegFilePtr reg_file)
    : SxxiInstructionInterface(instr, reg_file) {
  name_ = "srai";
}

////////////////////////////////////////////////////////////////////////////////
void SraiInstruction::Execute() {
  SxxiInstructionInterface::Execute();
  const int sign_carry_mask = (Rs1_->Data() & (1 << 31)) ? -1 : 0;
  const int sign_ext_mask = (sign_carry_mask & ((1 << shamt_) - 1))
                            << ((8 * sizeof(reg_data_t)) - shamt_);
  Rd_->Data() = Rs1_->Data() >> shamt_;
  Rd_->Data() |= sign_ext_mask;
}

////////////////////////////////////////////////////////////////////////////////
AddInstruction::AddInstruction(instr_t instr, RegFilePtr reg_file)
    : RTypeInstructionInterface(instr, reg_file) {
  name_ = "add";
}

////////////////////////////////////////////////////////////////////////////////
void AddInstruction::Execute() {
  Rd_->Data() = Rs1_->Data() + Rs2_->Data();
  RTypeInstructionInterface::Execute();
}

////////////////////////////////////////////////////////////////////////////////
SubInstruction::SubInstruction(instr_t instr, RegFilePtr reg_file)
    : RTypeInstructionInterface(instr, reg_file) {
  name_ = "sub";
}

////////////////////////////////////////////////////////////////////////////////
void SubInstruction::Execute() {
  Rd_->Data() = Rs1_->Data() - Rs2_->Data();
  RTypeInstructionInterface::Execute();
}

////////////////////////////////////////////////////////////////////////////////
SllInstruction::SllInstruction(instr_t instr, RegFilePtr reg_file)
    : RTypeInstructionInterface(instr, reg_file) {
  name_ = "sll";
}

////////////////////////////////////////////////////////////////////////////////
void SllInstruction::Execute() {
  Rd_->Data() = Rs1_->Data() << Rs2_->Data();
  RTypeInstructionInterface::Execute();
}

////////////////////////////////////////////////////////////////////////////////
SltInstruction::SltInstruction(instr_t instr, RegFilePtr reg_file)
    : RTypeInstructionInterface(instr, reg_file) {
  name_ = "slt";
}

////////////////////////////////////////////////////////////////////////////////
void SltInstruction::Execute() {
  Rd_->Data() = static_cast<signed_reg_data_t>(Rs1_->Data()) <
                static_cast<signed_reg_data_t>(Rs2_->Data());
  RTypeInstructionInterface::Execute();
}

////////////////////////////////////////////////////////////////////////////////
SltuInstruction::SltuInstruction(instr_t instr, RegFilePtr reg_file)
    : RTypeInstructionInterface(instr, reg_file) {
  name_ = "sltu";
}

////////////////////////////////////////////////////////////////////////////////
void SltuInstruction::Execute() {
  Rd_->Data() = Rs1_->Data() < Rs2_->Data();
  RTypeInstructionInterface::Execute();
}

////////////////////////////////////////////////////////////////////////////////
XorInstruction::XorInstruction(instr_t instr, RegFilePtr reg_file)
    : RTypeInstructionInterface(instr, reg_file) {
  name_ = "xor";
}

////////////////////////////////////////////////////////////////////////////////
void XorInstruction::Execute() {
  Rd_->Data() = Rs1_->Data() ^ Rs2_->Data();
  RTypeInstructionInterface::Execute();
}

////////////////////////////////////////////////////////////////////////////////
SrlInstruction::SrlInstruction(instr_t instr, RegFilePtr reg_file)
    : RTypeInstructionInterface(instr, reg_file) {
  name_ = "srl";
}

////////////////////////////////////////////////////////////////////////////////
void SrlInstruction::Execute() {
  Rd_->Data() = Rs1_->Data() >> Rs2_->Data();
  RTypeInstructionInterface::Execute();
}

////////////////////////////////////////////////////////////////////////////////
SraInstruction::SraInstruction(instr_t instr, RegFilePtr reg_file)
    : RTypeInstructionInterface(instr, reg_file) {
  name_ = "sra";
}

////////////////////////////////////////////////////////////////////////////////
void SraInstruction::Execute() {
  const int sign_carry_mask = (Rs1_->Data() & (1 << 31)) ? -1 : 0;
  const int sign_ext_mask = (sign_carry_mask & ((1 << Rs2_->Data()) - 1));
  Rd_->Data() = Rs1_->Data() >> Rs2_->Data();
  Rd_->Data() |= sign_ext_mask;
  RTypeInstructionInterface::Execute();
}

////////////////////////////////////////////////////////////////////////////////
OrInstruction::OrInstruction(instr_t instr, RegFilePtr reg_file)
    : RTypeInstructionInterface(instr, reg_file) {
  name_ = "or";
}

////////////////////////////////////////////////////////////////////////////////
void OrInstruction::Execute() {
  Rd_->Data() = Rs1_->Data() | Rs2_->Data();
  RTypeInstructionInterface::Execute();
}

////////////////////////////////////////////////////////////////////////////////
AndInstruction::AndInstruction(instr_t instr, RegFilePtr reg_file)
    : RTypeInstructionInterface(instr, reg_file) {
  name_ = "and";
}

////////////////////////////////////////////////////////////////////////////////
void AndInstruction::Execute() {
  Rd_->Data() = Rs1_->Data() & Rs2_->Data();
  RTypeInstructionInterface::Execute();
}

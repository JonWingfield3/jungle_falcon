#include <i_type_instructions.hpp>

#include <pipeline.hpp>

ITypeInstructionInterface::ITypeInstructionInterface(instr_t instr,
                                                     RegFilePtr reg_file)
    : InstructionInterface(instr), reg_file_(reg_file) {
  name_ = "I Type Instruction";
  instruction_type_ = InstructionTypes::IType;
}

void ITypeInstructionInterface::Decode() {
  ITypeInstructionFormat i_type_format;
  i_type_format.word = instr_;

  const int rd_num = i_type_format.rd;
  Rd_ = std::make_shared<Register>(Register(rd_num));
  reg_file_->Read(*Rd_);

  const int rs1_num = i_type_format.rs1;
  Rs1_ = std::make_shared<Register>(Register(rs1_num));
  reg_file_->Read(*Rs1_);

  const imm_t imm_upper_20 =
      ((i_type_format.imm11_0 & (1 << 11)) ? -1 : 0) & ~(0xfff);
  imm_ = static_cast<imm_t>(imm_upper_20 | i_type_format.imm11_0);

  InstructionInterface::Decode();
}

void ITypeInstructionInterface::Execute() {
  VLOG(3) << "Executed: " << Rd_ << " = " << name_ << "{" << Rs1_ << ", "
          << imm_ << "}";
  InstructionInterface::Execute();
}

void ITypeInstructionInterface::WriteBack() {
  VLOG(3) << name_ << " WriteBack stage: writing " << Rd_ << " back to regfile";
  reg_file_->Write(*Rd_);
  InstructionInterface::WriteBack();
}

void ITypeInstructionInterface::SetInstructionName() {
  std::stringstream instruction_stream;
  instruction_stream << name_ << " x" << Rd_->Number() << ", x"
                     << Rs1_->Number() << ", " << imm_;
  instruction_ = instruction_stream.str();
}

std::string ITypeInstructionInterface::RegistersString() {
  std::stringstream reg_str;
  reg_str << "rd: " << Rd() << ", rs1: " << Rs1();
  return reg_str.str();
}

////////////////////////////////////////////////////////////////////////////////
AddiInstruction::AddiInstruction(instr_t instr, RegFilePtr reg_file)
    : ITypeInstructionInterface(instr, reg_file) {
  name_ = "addi";
}

////////////////////////////////////////////////////////////////////////////////
void AddiInstruction::Execute() {
  Rd_->Data() = Rs1_->Data() + imm_;
  ITypeInstructionInterface::Execute();
}

////////////////////////////////////////////////////////////////////////////////
SltiInstruction::SltiInstruction(instr_t instr, RegFilePtr reg_file)
    : ITypeInstructionInterface(instr, reg_file) {
  name_ = "slti";
}

////////////////////////////////////////////////////////////////////////////////
void SltiInstruction::Execute() {
  Rd_->Data() = static_cast<signed_reg_data_t>(Rs1_->Data()) < imm_;
  ITypeInstructionInterface::Execute();
}

////////////////////////////////////////////////////////////////////////////////
SltiuInstruction::SltiuInstruction(instr_t instr, RegFilePtr reg_file)
    : ITypeInstructionInterface(instr, reg_file) {
  name_ = "sltiu";
}

////////////////////////////////////////////////////////////////////////////////
void SltiuInstruction::Execute() {
  Rd_->Data() = Rs1_->Data() < static_cast<unsigned>(imm_);
  ITypeInstructionInterface::Execute();
}

////////////////////////////////////////////////////////////////////////////////
XoriInstruction::XoriInstruction(instr_t instr, RegFilePtr reg_file)
    : ITypeInstructionInterface(instr, reg_file) {
  name_ = "xori";
}

////////////////////////////////////////////////////////////////////////////////
void XoriInstruction::Execute() {
  Rd_->Data() = Rs1_->Data() ^ imm_;
  ITypeInstructionInterface::Execute();
}

////////////////////////////////////////////////////////////////////////////////
OriInstruction::OriInstruction(instr_t instr, RegFilePtr reg_file)
    : ITypeInstructionInterface(instr, reg_file) {
  name_ = "ori";
}

////////////////////////////////////////////////////////////////////////////////
void OriInstruction::Execute() {
  Rd_->Data() = Rs1_->Data() | imm_;
  ITypeInstructionInterface::Execute();
}

////////////////////////////////////////////////////////////////////////////////
AndiInstruction::AndiInstruction(instr_t instr, RegFilePtr reg_file)
    : ITypeInstructionInterface(instr, reg_file) {
  name_ = "andi";
}

////////////////////////////////////////////////////////////////////////////////
void AndiInstruction::Execute() {
  Rd_->Data() = Rs1_->Data() & imm_;
  ITypeInstructionInterface::Execute();
}

////////////////////////////////////////////////////////////////////////////////
JalrInstruction::JalrInstruction(instr_t instr, RegFilePtr reg_file, PcPtr pc)
    : ITypeInstructionInterface(instr, reg_file), pc_(pc) {
  name_ = "jalr";
}

////////////////////////////////////////////////////////////////////////////////
void JalrInstruction::Execute() {
#if (__INSTRUCTION_ACCURATE__ == 1)
  const int pipeline_offset = sizeof(instr_t);
#else
  // TODO: determine pipeline correction here
  const int pipeline_offset =
      (Pipeline::Stages::DecodeStage - Pipeline::Stages::FetchStage + 1) *
      sizeof(instr_t);
#endif
  Rd_->Data() = pc_->Reg() + pipeline_offset;
  target_addr_ = (Rs1_->Data() + imm_) & ~1;
  VLOG(3) << "Execute: Target address = " << target_addr_;
  ITypeInstructionInterface::Execute();
}

////////////////////////////////////////////////////////////////////////////////
void JalrInstruction::MemoryAccess() {
  pc_->Jump(target_addr_);
  VLOG(1) << "WriteBack: Jumping to " << target_addr_;
  ITypeInstructionInterface::MemoryAccess();
}

////////////////////////////////////////////////////////////////////////////////
void JalrInstruction::WriteBack() {
  reg_file_->Write(*Rd_);
  InstructionInterface::WriteBack();
}

////////////////////////////////////////////////////////////////////////////////
LoadInstructionInterface::LoadInstructionInterface(instr_t instr,
                                                   RegFilePtr reg_file,
                                                   MemoryPtr mem)
    : ITypeInstructionInterface(instr, reg_file), mem_(mem) {
  name_ = "load";
}

////////////////////////////////////////////////////////////////////////////////
void LoadInstructionInterface::Execute() {
  load_addr_ = Rs1_->Data() + imm_;
  VLOG(3) << "Execute: calculated load address as " << load_addr_;
  InstructionInterface::Execute();
}

////////////////////////////////////////////////////////////////////////////////
void LoadInstructionInterface::MemoryAccess() {
  InstructionInterface::MemoryAccess();
}

////////////////////////////////////////////////////////////////////////////////
void LoadInstructionInterface::WriteBack() {
  reg_file_->Write(*Rd_);
  InstructionInterface::WriteBack();
}

////////////////////////////////////////////////////////////////////////////////
void LoadInstructionInterface::SetInstructionName() {
  std::stringstream instruction_stream;
  instruction_stream << name_ << " x" << static_cast<int>(Rd_->Number()) << ", "
                     << imm_ << "(x" << static_cast<int>(Rs1_->Number()) << ")";
  instruction_ = instruction_stream.str();
}

////////////////////////////////////////////////////////////////////////////////
LbInstruction::LbInstruction(instr_t instr, RegFilePtr reg_file, MemoryPtr mem)
    : LoadInstructionInterface(instr, reg_file, mem) {
  name_ = "lb";
}

////////////////////////////////////////////////////////////////////////////////
void LbInstruction::MemoryAccess() {
  Rd_->Data() = static_cast<signed_reg_data_t>(mem_->ReadByte(load_addr_));
  LoadInstructionInterface::MemoryAccess();
}

////////////////////////////////////////////////////////////////////////////////
LbuInstruction::LbuInstruction(instr_t instr, RegFilePtr reg_file,
                               MemoryPtr mem)
    : LoadInstructionInterface(instr, reg_file, mem) {
  name_ = "lbu";
}

////////////////////////////////////////////////////////////////////////////////
void LbuInstruction::MemoryAccess() {
  Rd_->Data() = static_cast<reg_data_t>(mem_->ReadByte(load_addr_));
  LoadInstructionInterface::MemoryAccess();
}

////////////////////////////////////////////////////////////////////////////////
LhInstruction::LhInstruction(instr_t instr, RegFilePtr reg_file, MemoryPtr mem)
    : LoadInstructionInterface(instr, reg_file, mem) {
  name_ = "lh";
}

////////////////////////////////////////////////////////////////////////////////
void LhInstruction::MemoryAccess() {
  Rd_->Data() = static_cast<signed_reg_data_t>(mem_->ReadHalfWord(load_addr_));
  LoadInstructionInterface::MemoryAccess();
}

////////////////////////////////////////////////////////////////////////////////
LhuInstruction::LhuInstruction(instr_t instr, RegFilePtr reg_file,
                               MemoryPtr mem)
    : LoadInstructionInterface(instr, reg_file, mem) {
  name_ = "lhu";
}

////////////////////////////////////////////////////////////////////////////////
void LhuInstruction::MemoryAccess() {
  Rd_->Data() = static_cast<reg_data_t>(mem_->ReadHalfWord(load_addr_));
  LoadInstructionInterface::MemoryAccess();
}

////////////////////////////////////////////////////////////////////////////////
LwInstruction::LwInstruction(instr_t instr, RegFilePtr reg_file, MemoryPtr mem)
    : LoadInstructionInterface(instr, reg_file, mem) {
  name_ = "lw";
}

////////////////////////////////////////////////////////////////////////////////
void LwInstruction::MemoryAccess() {
  Rd_->Data() = static_cast<reg_data_t>(mem_->ReadWord(load_addr_));
  LoadInstructionInterface::MemoryAccess();
}

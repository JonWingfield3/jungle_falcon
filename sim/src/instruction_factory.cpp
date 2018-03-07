#include <instruction_factory.hpp>

#include <map>
#include <memory>

#include <b_type_instructions.hpp>
#include <i_type_instructions.hpp>
#include <j_type_instructions.hpp>
#include <r_type_instructions.hpp>
#include <s_type_instructions.hpp>
#include <u_type_instructions.hpp>

InstructionFactory::InstructionPtr InstructionFactory::Create(instr_t instr) {
  InstructionInterface::GenericInstructionFormat generic_instr_format;
  generic_instr_format.word = instr;

  OpCode op = static_cast<OpCode>(generic_instr_format.opcode);
  VLOG(4) << "Instruction opcode = " << std::showbase << std::hex
          << generic_instr_format.opcode << "   " << generic_instr_format.upper;
  switch (op) {
    case OpCode::LUI:
      break;
    case OpCode::AUIPC:
      break;
    case OpCode::JAL:
      break;
    case OpCode::JALR:
    case OpCode::Lx:
    case OpCode::Bxx:
      break;
    case OpCode::ITypeArithmeticAndLogical: {
      ITypeInstructionInterface::ITypeInstructionFormat i_type_format;
      i_type_format.word = instr;
      Funct3 funct3 = static_cast<Funct3>(i_type_format.funct3);
      switch (funct3) {
        case Funct3::ADDI:
          return std::make_shared<AddiInstruction>(
              AddiInstruction(instr, reg_file_));
        case Funct3::SLTI:
          return std::make_shared<SltiInstruction>(
              SltiInstruction(instr, reg_file_));
        case Funct3::SLTIU:
          return std::make_shared<SltiuInstruction>(
              SltiuInstruction(instr, reg_file_));
        case Funct3::XORI:
          return std::make_shared<XoriInstruction>(
              XoriInstruction(instr, reg_file_));
        case Funct3::ORI:
          return std::make_shared<OriInstruction>(
              OriInstruction(instr, reg_file_));
        case Funct3::ANDI:
          return std::make_shared<AndiInstruction>(
              AndiInstruction(instr, reg_file_));
        case Funct3::SLLI:
          return std::make_shared<SlliInstruction>(
              SlliInstruction(instr, reg_file_));
        case Funct3::SRLI:  // || SRAI
          if (funct3 == Funct3::SRLI) {
            return std::make_shared<SrliInstruction>(
                SrliInstruction(instr, reg_file_));
          } else {
            return std::make_shared<SraiInstruction>(
                SraiInstruction(instr, reg_file_));
          }
      }
    } break;
    case OpCode::RTypeArithmeticAndLogical: {
      RTypeInstructionInterface::RTypeInstructionFormat r_type_format;
      r_type_format.word = instr;
      Funct3 funct3 = static_cast<Funct3>(r_type_format.funct3);
      switch (funct3) {
        case Funct3::ADD:
          return std::make_shared<AddInstruction>(
              AddInstruction(instr, reg_file_));
        case Funct3::SLL:
          return std::make_shared<SllInstruction>(
              SllInstruction(instr, reg_file_));
        case Funct3::SLT:
          return std::make_shared<SltInstruction>(
              SltInstruction(instr, reg_file_));
        case Funct3::SLTU:
          return std::make_shared<SltuInstruction>(
              SltuInstruction(instr, reg_file_));
        case Funct3::XOR:
          return std::make_shared<XorInstruction>(
              XorInstruction(instr, reg_file_));
        case Funct3::SRL:
          return std::make_shared<SrlInstruction>(
              SrlInstruction(instr, reg_file_));
        case Funct3::OR:
          return std::make_shared<OrInstruction>(
              OrInstruction(instr, reg_file_));
        case Funct3::AND:
          return std::make_shared<AndInstruction>(
              AndInstruction(instr, reg_file_));
      }
    } break;
    case OpCode::Sx: {
      STypeInstructionInterface::STypeInstructionFormat s_type_format;
      s_type_format.word = instr;
      const Funct3 funct3 = static_cast<Funct3>(s_type_format.funct3);
      switch (funct3) {
        case Funct3::SB:
          return std::make_shared<SbInstruction>(
              SbInstruction(instr, reg_file_, mem_));
        case Funct3::SH:
          return std::make_shared<ShInstruction>(
              ShInstruction(instr, reg_file_, mem_));
        case Funct3::SW:
          return std::make_shared<SwInstruction>(
              SwInstruction(instr, reg_file_, mem_));
      }
    } break;
    default:
      LOG(FATAL) << "Unable to match instruction!";
      break;
  }
  return nullptr;
}

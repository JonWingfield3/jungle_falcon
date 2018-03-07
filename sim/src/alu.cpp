#include <alu.hpp>

#include <instructions.hpp>
#include <pipeline.hpp>
#include <riscv_defs.hpp>

reg_data_t ALU::Exec(Funct3 funct3, reg_data_t op1, reg_data_t op2,
                     Funct7 funct7) {
  switch (funct3) {
    case Funct3::ADD:
      if (funct7 == Funct7::SUB) {
        return op1 - op2;
      } else {
        return op1 + op2;
      }
    case Funct3::SLT:  // case Funct3::SLTI:
      return static_cast<reg_data_t>(static_cast<signed_reg_data_t>(op1) <
                                     static_cast<signed_reg_data_t>(op2));
    case Funct3::AND:  // case Funct3::ANDI:
      return op1 & op2;
    case Funct3::XOR:  // case Funct3::XORI:
      return op1 ^ op2;
    case Funct3::OR:  // case Funct3::ORI:
      return op1 | op2;
    case Funct3::SRL:  // == SRA == SRLI == SRAI
      if (funct7 == Funct7::SRLI || funct7 == Funct7::SRL) {
        return op1 >> op2;
      } else if (funct7 == Funct7::SRAI || funct7 == Funct7::SRA) {
        return static_cast<reg_data_t>(static_cast<signed_reg_data_t>(op1) >>
                                       op2);
      } else {
        LOG(FATAL) << "Unmatched SRx";
      }
    case Funct3::SLL:  // case Funct3::SLLI:
      return op1 << op2;
    case Funct3::SLTU:  // case Funct3::SLTIU:
      return static_cast<reg_data_t>(op1 < op2);
    default:
      LOG(FATAL) << "Unmatched ALU Op Code";
  }
}

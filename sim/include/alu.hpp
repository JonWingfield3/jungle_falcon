#pragma once

#include <functional>
#include <map>

#include <instructions.hpp>
#include <register_file.hpp>
#include <riscv_defs.hpp>

class ALU {
 public:
  reg_data_t Exec(Funct3 funct3, reg_data_t op1, reg_data_t op2,
                  Funct7 funct7 = Funct7::XXX);

 private:
};

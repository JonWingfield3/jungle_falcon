#include <register_file.hpp>

#include <gtest/gtest.h>

#include <array>
#include <iostream>

class RegisterFileTestClass {
  //  RegisterFileTestClass(RegisterFile register_file);
  static void DumpRegisters(RegisterFile register_file,
                            std::vector<RegisterFile::Register> registers,
                            const std::string format = "hex"){};
};

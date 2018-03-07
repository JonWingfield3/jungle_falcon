#include <iostream>

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <gtest/gtest.h>

#include <instruction_factory.hpp>
#include <instructions.hpp>
#include <memory.hpp>
#include <r_type_instructions.hpp>
#include <register_file.hpp>

TEST(register_file_tests, reg_read_write_test) {
  RegisterFile register_file;
  LOG(INFO) << "Running read write tests!";
}

TEST(memory_tests, mem_read_write_test) {
  constexpr int kTestMemSize = 100;
  MainMemory mem(static_cast<mem_addr_t>(kTestMemSize));
  for (std::size_t ii = 0; ii < kTestMemSize; ++ii) {
    mem.Write(ii, static_cast<uint8_t>(ii));
  }
  MemoryUtils::CoreDump(mem);
}

TEST(memory_tests, mem_load_image_test) {
  const std::string test_bin = "asm/test.bin";
  MainMemory main_memory;
  MemoryUtils::LoadImage(main_memory, test_bin);
  MemoryUtils::CoreDump(main_memory);
}

TEST(pipeline_tests, addi_test) {
  const std::string addi_test_bin = "asm/addi.bin";
  MainMemory main_mem;
  MemoryUtils::LoadImage(main_mem, addi_test_bin);
  MemoryUtils::CoreDump(main_mem);
  ProgramCounter pc;
  RegisterFile reg_file;
  InstructionFactory factory(reg_file, pc, main_mem);

  VLOG(1) << "Pc = " << pc.pc();
  const instr_t instr = main_mem.Read<instr_t>(pc.pc());
  VLOG(1) << "Read value " << std::hex << instr;

  InstructionFactory::InstructionPtr instr_ptr = factory.Create(instr);
  instr_ptr->Run();
}

int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  google::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);
  return RUN_ALL_TESTS();
}

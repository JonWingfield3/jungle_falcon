#include <iostream>

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <gtest/gtest.h>

//#include <command_interpreter.hpp>
//#include <commands.hpp>
//#include <cpu.hpp>
#include <instruction_factory.hpp>
#include <instructions.hpp>
#include <memory.hpp>
#include <r_type_instructions.hpp>
#include <register_file.hpp>

TEST(memory_tests, mem_read_write_test) {
  constexpr int kTestMemSize = 100;
  MemoryPtr mem = std::make_shared<MainMemory>(MainMemory("", kTestMemSize));
  for (std::size_t ii = 0; ii < kTestMemSize; ++ii) {
    mem->WriteByte(ii, static_cast<uint8_t>(ii));
  }
  mem->CoreDump(kTestMemSize);
}

TEST(memory_tests, mem_load_image_test) {
  const std::string test_bin = "asm/test.bin";
  MemoryPtr mem = std::make_shared<MainMemory>(MainMemory(test_bin));
  mem->CoreDump(100);
}

TEST(pipeline_tests, addi_test) {
  const std::string addi_test_bin = "asm/addi.bin";
  std::shared_ptr<MemoryBase> mem =
      std::make_shared<MainMemory>(MainMemory(addi_test_bin));
  PcPtr pc = std::make_shared<ProgramCounter>(ProgramCounter());
  RegFilePtr reg_file = std::make_shared<RegisterFile>(RegisterFile());
  InstructionFactory factory(reg_file, pc, mem);
  const instr_t instr = mem->ReadWord(pc->Reg());
  InstructionFactory::InstructionPtr instr_ptr = factory.Create(instr);
  instr_ptr->ExecuteCycle();
}

TEST(command_test, factory_test) {
  const std::string q = "q";
  const std::string dr = "dr";
  const std::string dm = "dm";
  const std::string pi = "pi";
  const std::string c = "c";

  const std::string test_bin = "asm/test.bin";
  //  MemoryPtr mem = std::make_shared<MainMemory>(MainMemory(test_bin));
  //  CpuPtr cpu = std::make_shared<CPU>(Cpu(mem));

  //  CommandInterpreter interp(cpu, mem);
}

int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  google::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);
  return RUN_ALL_TESTS();
}

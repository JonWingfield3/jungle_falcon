#include <iostream>

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <gtest/gtest.h>
#include <chrono>
#include <random>

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
  MemoryPtr mem =
      std::make_shared<InstructionMemory>(InstructionMemory("asm/addi.bin"));
  for (std::size_t ii = 0; ii < kTestMemSize; ++ii) {
    mem->WriteByte(ii, static_cast<uint8_t>(ii));
  }
  mem->CoreDump(kTestMemSize);
}

TEST(memory_tests, mem_load_image_test) {
  const std::string test_bin = "asm/test.bin";
  MemoryPtr mem =
      std::make_shared<InstructionMemory>(InstructionMemory(test_bin));
  mem->CoreDump(100);
}

TEST(pipeline_tests, addi_test) {
  const std::string addi_test_bin = "asm/addi.bin";
  std::shared_ptr<MemoryBase> mem =
      std::make_shared<InstructionMemory>(InstructionMemory(addi_test_bin));
  PcPtr pc = std::make_shared<ProgramCounter>(ProgramCounter());
  RegFilePtr reg_file = std::make_shared<RegisterFile>(RegisterFile());
  InstructionFactory factory(reg_file, pc, mem);
  const instr_t instr = mem->ReadWord(pc->Reg());
  const InstructionPtr instr_ptr = factory.Create(instr);
  instr_ptr->ExecuteCycle(0);
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

TEST(cache_tests, dm_cache_rw) {
  MemoryPtr test_mem = std::make_shared<DataMemory>(DataMemory());
  MemoryPtr cache = std::make_shared<DirectlyMappedCache>(
      DirectlyMappedCache(test_mem, 4, 1));

  constexpr int NUM_LOOPS{64};
  for (int ii = 0; ii < NUM_LOOPS; ++ii) {
    test_mem->WriteByte(ii, ii);
  }

  test_mem->CoreDump(0, NUM_LOOPS);

  for (int ii = 0; ii < NUM_LOOPS; ++ii) {
    const int byte = cache->ReadByte(ii);
    CHECK(byte == ii) << "Read incorrect data! (" << byte << " != " << ii
                      << ")";
    VLOG(1) << "Address: " << ii << ", Data: " << byte;

    const int write_byte = NUM_LOOPS - ii;
    cache->WriteByte(ii, write_byte);
  }
  cache->WriteByte(0, 63);

  for (int ii = 0; ii < NUM_LOOPS; ++ii) {
    const int byte = test_mem->ReadByte(ii);
    CHECK(byte == (NUM_LOOPS - ii)) << "Read incorrect data! (" << byte
                                    << " != " << (NUM_LOOPS - ii) << ")";
  }
}

TEST(cache_tests, directly_mapped_cache_test) {
  MemoryPtr test_mem = std::make_shared<DataMemory>(DataMemory());
  MemoryPtr cache =
      std::make_shared<DirectlyMappedCache>(DirectlyMappedCache(test_mem));

  std::mt19937 rng;
  std::uniform_int_distribution<uint32_t> udist1024(0, 1024);
  rng.seed(0xdeadbeef);

  for (int ii = 0; ii < 100; ++ii) {
    const int next_data = udist1024(rng);
    const int next_addr = udist1024(rng);
  }
}

int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  google::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);
  return RUN_ALL_TESTS();
}

#include <iostream>

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <gtest/gtest.h>
#include <chrono>
#include <random>

//#include <command_interpreter.hpp>
//#include <commands.hpp>
#include <cpu.hpp>
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
  const mem_addr_t instruction_pointer = pc->InstructionPointer();
  const instr_t instr = mem->ReadWord(instruction_pointer);
  const InstructionPtr instr_ptr = factory.Create(instr);
  instr_ptr->ExecuteCycle(0);
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
    const int write_byte = NUM_LOOPS - ii;
    cache->WriteByte(ii, write_byte);
  }
  // Force cache write back
  cache->WriteByte(0, 63);

  for (int ii = 0; ii < NUM_LOOPS; ++ii) {
    const int byte = test_mem->ReadByte(ii);
    CHECK(byte == (NUM_LOOPS - ii)) << "Read incorrect data! (" << byte
                                    << " != " << (NUM_LOOPS - ii) << ")";
  }
}

TEST(cache_tests, dm_cache_rw2) {
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
    const int write_byte = NUM_LOOPS - ii;
    cache->WriteByte(ii, write_byte);
  }
  // Force cache write back
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
  rng.seed(0xdeadbeef);  // for more reproducible results

  for (int ii = 0; ii < 100; ++ii) {
    const int next_data = udist1024(rng);
    const int next_addr = udist1024(rng);
    VLOG(1) << "Writing " << next_data << " to address: " << next_addr;
    cache->WriteWord(next_addr, next_data);
  }
}

TEST(cache_tests, lru_cache) {
  MemoryPtr test_mem = std::make_shared<DataMemory>(DataMemory(128));
  // Purely associative cache.
  MemoryPtr cache = std::make_shared<LRUCache>(LRUCache(test_mem, 4, 4, 4));

  constexpr int NUM_LOOPS{64};
  for (int ii = 0; ii < NUM_LOOPS; ++ii) {
    test_mem->WriteByte(ii, ii);
  }

  test_mem->CoreDump(0, NUM_LOOPS);

  for (int ii = 0; ii < NUM_LOOPS; ++ii) {
    const int byte = cache->ReadByte(ii);
    CHECK(byte == ii) << "Read incorrect data! (" << byte << " != " << ii
                      << ")";
    const int write_byte = NUM_LOOPS - ii;
    cache->WriteByte(ii, write_byte);
  }
  // Force cache write back
  cache->WriteByte(0, 63);

  for (int ii = 0; ii < NUM_LOOPS; ++ii) {
    const int byte = test_mem->ReadByte(ii);
    CHECK(byte == (NUM_LOOPS - ii)) << "Read incorrect data! (" << byte
                                    << " != " << (NUM_LOOPS - ii) << ")";
  }
}

TEST(program_tests, ins_assembly_test_no_cache) {
  const std::string test_bin = "asm/ins_assembly_test.bin";
  MemoryPtr instr_mem =
      std::make_shared<InstructionMemory>(InstructionMemory(test_bin));
  MemoryPtr data_mem = std::make_shared<DataMemory>(DataMemory());

  CpuPtr cpu = std::make_shared<CPU>(CPU(instr_mem, data_mem));

  const mem_addr_t pass_loop_addr = 0x52c;
  while (cpu->GetPC()->InstructionPointer() != pass_loop_addr) {
    cpu->ExecuteCycle();
  }

  CHECK(cpu->GetRegFile()->Read(RegisterFile::Registers::X25) == 0x1)
      << "Failed ins_assembly_test!";
  VLOG(1) << "Passed!";
}

TEST(program_tests, ins_assembly_test_with_cache) {
  const std::string test_bin = "asm/ins_assembly_test.bin";
  MemoryPtr instr_mem =
      std::make_shared<InstructionMemory>(InstructionMemory(test_bin));
  MemoryPtr data_mem = std::make_shared<DataMemory>(DataMemory());

  // Init caches
  MemoryPtr instr_cache = std::make_shared<DirectlyMappedCache>(
      DirectlyMappedCache(instr_mem, 16, 16));
  MemoryPtr data_cache = std::make_shared<DirectlyMappedCache>(
      DirectlyMappedCache(data_mem, 16, 16));

  CpuPtr cpu = std::make_shared<CPU>(CPU(instr_cache, data_cache));

  const mem_addr_t pass_loop_addr = 0x52c;
  while (cpu->GetPC()->InstructionPointer() != pass_loop_addr) {
    cpu->ExecuteCycle();
  }
  CHECK(cpu->GetRegFile()->Read(RegisterFile::Registers::X25) != 0x1)
      << "Failed ins_assembly_test!";
}

int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  google::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);
  return RUN_ALL_TESTS();
}

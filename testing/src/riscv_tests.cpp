#include <iostream>

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <gtest/gtest.h>
#include <chrono>
#include <random>

#include <command_interpreter.hpp>
#include <commands.hpp>
#include <cpu.hpp>
#include <instruction_factory.hpp>
#include <instructions.hpp>
#include <memory.hpp>
#include <r_type_instructions.hpp>
#include <register_file.hpp>

DEFINE_uint32(cache_line_size, 4, "Cache line size in words");
DEFINE_uint32(set_associativity, 2, "Set associativity of cache");
DEFINE_uint32(num_cache_lines, 16, "Number of lines in cache");
DEFINE_uint32(cache_latency, 1, "Number of cycles for cache hit");
DEFINE_uint32(
    first_word_latency, 10,
    "Number of cycles needed to access first word in a line from memory");
DEFINE_uint32(
    subsequent_word_latency, 1,
    "Number of cycles needed to access subsequent words in a line from memory");

TEST(memory_tests, mem_read_write_test) {
  constexpr int kTestMemSize = 100;
  MemoryPtr mem = std::make_shared<InstructionMemory>(
      InstructionMemory("asm/addi.bin", 0, 0));
  for (std::size_t ii = 0; ii < kTestMemSize; ++ii) {
    mem->WriteByte(ii, static_cast<uint8_t>(ii));
  }
  mem->CoreDump(kTestMemSize);
}

TEST(memory_tests, mem_load_image_test) {
  const std::string test_bin = "asm/test.bin";
  MemoryPtr mem =
      std::make_shared<InstructionMemory>(InstructionMemory(test_bin, 0, 0));
  mem->CoreDump(100);
}

TEST(pipeline_tests, addi_test) {
  const std::string addi_test_bin = "asm/addi.bin";
  std::shared_ptr<MemoryBase> mem = std::make_shared<InstructionMemory>(
      InstructionMemory(addi_test_bin, 0, 0));
  PcPtr pc = std::make_shared<ProgramCounter>(ProgramCounter());
  RegFilePtr reg_file = std::make_shared<RegisterFile>(RegisterFile());
  InstructionFactory factory(reg_file, pc, mem);
  const mem_addr_t instruction_pointer = pc->InstructionPointer();
  const instr_t instr = mem->ReadWord(instruction_pointer);
  const InstructionPtr instr_ptr = factory.Create(instr);
  instr_ptr->ExecuteCycle(0);
}

//
// Tests directly_mapped_cache implementation by filling memory, reading values
// through cache, writing new values to cache, and then checking memory
//
TEST(cache_tests, dm_cache_rw) {
  MemoryPtr test_mem = std::make_shared<DataMemory>(DataMemory(0, 0));
  MemoryPtr cache = std::make_shared<DirectlyMappedCache>(
      DirectlyMappedCache(test_mem, 4, 1, 0, 0));

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

//
// Tests lru_cache implementation by filling memory, reading values
// through cache, writing new values to cache, and then checking memory
//
TEST(cache_tests, lru_associative_cache_rw) {
  const std::size_t CACHE_LINE_SIZE{sizeof(word_t)};
  const std::size_t SET_ASSOCIATIVITY{4};
  const std::size_t NUM_CACHE_LINES{4};
  const std::size_t CACHE_LATENCY{FLAGS_cache_latency};
  const std::size_t MEMORY_FIRST_WORD_LATENCY{FLAGS_first_word_latency};
  const std::size_t MEMORY_SUBSEQUENT_WORD_LATENCY{
      FLAGS_subsequent_word_latency};

  VLOG(1) << "---------------------------------------------------------";
  VLOG(1) << "Creating memory with specs: ";
  VLOG(1) << "First word latency: " << MEMORY_FIRST_WORD_LATENCY;
  VLOG(1) << "Subsequent word latency: " << MEMORY_SUBSEQUENT_WORD_LATENCY;

  MemoryPtr test_mem =
      std::make_shared<DataMemory>(DataMemory(MEMORY_FIRST_WORD_LATENCY));

  VLOG(1) << "---------------------------------------------------------";
  VLOG(1) << "Creating cache with specs: ";
  VLOG(1) << "Line size: " << CACHE_LINE_SIZE;
  VLOG(1) << "Number of lines: " << NUM_CACHE_LINES;
  VLOG(1) << "Set associativity: " << SET_ASSOCIATIVITY;
  VLOG(1) << "Latency: " << CACHE_LATENCY;

  MemoryPtr cache = std::make_shared<LRUCache>(
      LRUCache(test_mem, CACHE_LINE_SIZE, NUM_CACHE_LINES, SET_ASSOCIATIVITY,
               CACHE_LATENCY, MEMORY_SUBSEQUENT_WORD_LATENCY));

  VLOG(1)
      << "Setting words at memory addresses 0 - 255 with the values 0 - 63...";
  constexpr int NUM_LOOPS{64};
  for (int ii = 0; ii < NUM_LOOPS; ++ii) {
    test_mem->WriteWord(sizeof(word_t) * ii, ii);
  }

  for (int ii = 0; ii < 16; ++ii) {
    VLOG(1) << "Address: " << sizeof(word_t) * ii;
    cache->ExecuteCycle();
    const word_t word = cache->ReadWord(sizeof(word_t) * ii);
    CHECK(word == ii) << "Read incorrect data! (" << word << " != " << ii
                      << ")";
  }

  for (int jj = 0; jj < 2; ++jj) {
    for (int ii = 0; ii < 2; ++ii) {
      cache->ExecuteCycle();
      VLOG(1) << "Address: " << sizeof(word_t) * ii;
      const word_t word = cache->ReadWord(sizeof(word_t) * ii);
      CHECK(word == ii) << "Read incorrect data! (" << word << " != " << ii
                        << ")";
    }
  }
}

TEST(program_tests, ins_assembly_test_no_cache) {
  constexpr std::size_t LATENCY{0};
  const std::string test_bin = "asm/ins_assembly_test.bin";

  MemoryPtr instr_mem = std::make_shared<InstructionMemory>(
      InstructionMemory(test_bin, LATENCY, LATENCY));
  MemoryPtr data_mem =
      std::make_shared<DataMemory>(DataMemory(LATENCY, LATENCY));

  CpuPtr cpu = std::make_shared<CPU>(CPU(instr_mem, data_mem));

  constexpr mem_addr_t PASS_LOOP_ADDR{0x53c};
  const PcPtr pc = cpu->GetPC();
  while (pc->InstructionPointer() < PASS_LOOP_ADDR) {
    cpu->ExecuteCycle();
  }

  const RegFilePtr reg_file = cpu->GetRegFile();
  constexpr RegisterFile::Registers PASS_REG{RegisterFile::Registers::X25};
  constexpr std::size_t PASS_VAL{0x1};
  CHECK(reg_file->Read(PASS_REG) == PASS_VAL) << "Failed ins_assembly_test!";
  VLOG(1) << "Passed!";
}

TEST(program_tests, ins_assembly_test_with_cache) {
  const std::vector<std::size_t> CACHE_LINE_SIZES{
      1 * sizeof(word_t), 4 * sizeof(word_t), 16 * sizeof(word_t)};
  const std::vector<std::size_t> SET_ASSOCIATIVITIES{1, 2, 4};
  const std::vector<std::size_t> CACHE_LATENCIES{1, 2};
  const std::vector<std::size_t> FIRST_WORD_LATENCIES{10, 20};
  const std::vector<std::size_t> SUBSEQUENT_WORD_LATENCIES{1, 2};
  const std::size_t NUM_CACHE_LINES{FLAGS_num_cache_lines};
  const std::string test_bin = "asm/ins_assembly_test.bin";

  std::size_t test_counter = 0;
  for (std::size_t CACHE_LINE_SIZE : CACHE_LINE_SIZES) {
    for (std::size_t SET_ASSOCIATIVITY : SET_ASSOCIATIVITIES) {
      for (std::size_t CACHE_LATENCY : CACHE_LATENCIES) {
        for (std::size_t MEMORY_FIRST_WORD_LATENCY : FIRST_WORD_LATENCIES) {
          for (std::size_t MEMORY_SUBSEQUENT_WORD_LATENCY :
               SUBSEQUENT_WORD_LATENCIES) {
            VLOG(0) << "------------------------------------------------ "
                    << test_counter++
                    << " ------------------------------------------------";

            VLOG(0) << "Creating memory with specs: First word latency: "
                    << MEMORY_FIRST_WORD_LATENCY
                    << ", Subsequent word latency: "
                    << MEMORY_SUBSEQUENT_WORD_LATENCY;

            MemoryPtr instr_mem = std::make_shared<InstructionMemory>(
                InstructionMemory(test_bin, MEMORY_FIRST_WORD_LATENCY));

            MemoryPtr data_mem = std::make_shared<DataMemory>(
                DataMemory(MEMORY_FIRST_WORD_LATENCY));

            VLOG(0) << "Creating cache with specs: Line size: "
                    << CACHE_LINE_SIZE
                    << ", Number of lines: " << NUM_CACHE_LINES
                    << ", Set associativity: " << SET_ASSOCIATIVITY
                    << ", Latency: " << CACHE_LATENCY;

            // Init caches
            MemoryPtr instr_cache = std::make_shared<LRUCache>(LRUCache(
                instr_mem, CACHE_LINE_SIZE, NUM_CACHE_LINES, SET_ASSOCIATIVITY,
                CACHE_LATENCY, MEMORY_SUBSEQUENT_WORD_LATENCY));

            MemoryPtr data_cache = std::make_shared<LRUCache>(LRUCache(
                data_mem, CACHE_LINE_SIZE, NUM_CACHE_LINES, SET_ASSOCIATIVITY,
                CACHE_LATENCY, MEMORY_SUBSEQUENT_WORD_LATENCY));

            CpuPtr cpu = std::make_shared<CPU>(CPU(instr_cache, data_cache));

            constexpr mem_addr_t PASS_LOOP_ADDR{0x53c};
            const PcPtr pc = cpu->GetPC();
            while (pc->InstructionPointer() != PASS_LOOP_ADDR) {
              cpu->ExecuteCycle();
            }

            const RegFilePtr reg_file = cpu->GetRegFile();
            constexpr RegisterFile::Registers PASS_REG{
                RegisterFile::Registers::X25};
            constexpr std::size_t PASS_VAL{0x1};
            CHECK(reg_file->Read(PASS_REG) == PASS_VAL)
                << "Failed ins_assembly_test!";
            VLOG(0) << "Num cycles: " << cpu->GetCycles()
                    << ", CPI: " << cpu->GetCPI();
          }
        }
      }
    }
  }
}

int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  google::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);
  return RUN_ALL_TESTS();
}

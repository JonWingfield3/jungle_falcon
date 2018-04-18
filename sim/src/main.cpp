#include <gflags/gflags.h>
#include <glog/logging.h>

#include <iostream>
#include <memory>

#include <command_interpreter.hpp>
#include <cpu.hpp>
#include <memory.hpp>

// Program to execute
DEFINE_string(riscv_binary, "", "Program to run in simulator");

// Cache and memory parameters
DEFINE_uint32(cache_line_size, 4, "Cache line size in words");
DEFINE_uint32(set_associativity, 2, "Set associativity of cache");
DEFINE_uint32(num_cache_lines, 4, "Number of lines in cache");
DEFINE_uint32(cache_latency, 1, "Number of cycles for cache hit");
DEFINE_uint32(
    first_word_latency, 10,
    "Number of cycles needed to access first word in a line from memory");
DEFINE_uint32(
    subsequent_word_latency, 1,
    "Number of cycles needed to access subsequent words in a line from memory");

int main(int argc, char* argv[]) {
  google::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);

  // Read in memory params and init memories
  const std::size_t FIRST_WORD_LATENCY{FLAGS_first_word_latency};

  MemoryPtr instr_mem = std::make_shared<InstructionMemory>(
      InstructionMemory(FLAGS_riscv_binary, FIRST_WORD_LATENCY));
  MemoryPtr data_mem =
      std::make_shared<DataMemory>(DataMemory(FIRST_WORD_LATENCY));

  // Read in cache params and init caches
  const std::size_t LINE_SIZE{FLAGS_cache_line_size * sizeof(word_t)};
  const std::size_t NUM_LINES{FLAGS_num_cache_lines};
  const std::size_t CACHE_LATENCY{FLAGS_cache_latency};
  const std::size_t SET_ASSOCIATIVITY{FLAGS_set_associativity};
  const std::size_t SUBSEQUENT_WORD_LATENCY{FLAGS_subsequent_word_latency};

  MemoryPtr instr_cache = std::make_shared<LRUCache>(
      LRUCache(instr_mem, LINE_SIZE, NUM_LINES, SET_ASSOCIATIVITY,
               CACHE_LATENCY, SUBSEQUENT_WORD_LATENCY));
  MemoryPtr data_cache = std::make_shared<LRUCache>(
      LRUCache(data_mem, LINE_SIZE, NUM_LINES, SET_ASSOCIATIVITY, CACHE_LATENCY,
               SUBSEQUENT_WORD_LATENCY));

  // Init CPU
  CpuPtr cpu = std::make_shared<CPU>(CPU(instr_cache, data_cache));

  // Init interpreter
  CommandInterpreter interpreter(cpu, instr_mem, data_mem);

  interpreter.MainLoop();
  return 0;
}

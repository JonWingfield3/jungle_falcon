#include <gflags/gflags.h>
#include <glog/logging.h>

#include <iostream>
#include <memory>

#include <command_interpreter.hpp>
#include <cpu.hpp>
#include <memory.hpp>

DEFINE_string(riscv_binary, "", "Program to run in simulator");

int main(int argc, char* argv[]) {
  google::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);

  // Init main memories
  MemoryPtr instr_mem = std::make_shared<InstructionMemory>(
      InstructionMemory(FLAGS_riscv_binary, 100));
  MemoryPtr data_mem = std::make_shared<DataMemory>(DataMemory(100));

  // Init caches
  MemoryPtr instr_cache = std::make_shared<DirectlyMappedCache>(
      DirectlyMappedCache(instr_mem, 4, 4, 2));
  MemoryPtr data_cache = std::make_shared<DirectlyMappedCache>(
      DirectlyMappedCache(data_mem, 4, 4, 2));

  CpuPtr cpu = std::make_shared<CPU>(CPU(instr_cache, data_cache));
  CommandInterpreter interpreter(cpu, instr_mem, data_mem);

  interpreter.MainLoop();
  return 0;
}

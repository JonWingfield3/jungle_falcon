#include <gflags/gflags.h>
#include <glog/logging.h>

#include <iostream>
#include <memory>

#include <command_interpreter.hpp>
#include <cpu.hpp>
#include <memory.hpp>
#include <register_file.hpp>

DEFINE_string(riscv_binary, "", "Program to run in simulator");
DEFINE_uint32(stack_size, 65535, "Stack size for sim");

int main(int argc, char* argv[]) {
  google::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);

  const std::string image_name = FLAGS_riscv_binary;
  VLOG(1) << "Executing program: " << image_name << std::endl;

  MemoryPtr mem =
      std::make_shared<MainMemory>(MainMemory(image_name, FLAGS_stack_size));
  CpuPtr cpu = std::make_shared<CPU>(CPU(mem));
  CommandInterpreter interpreter(cpu, mem);

  interpreter.MainLoop();
  return 0;
}

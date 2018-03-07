#include <gflags/gflags.h>
#include <glog/logging.h>

#include <fstream>
#include <iostream>
#include <memory>

#include <memory.hpp>
#include <register_file.hpp>

DEFINE_string(riscv_binary, "", "Program to run in simulator");

int main(int argc, char* argv[]) {
  google::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);

  const std::string bin_image = FLAGS_riscv_binary;
  VLOG(5) << "Executing program: " << FLAGS_riscv_binary << std::endl;
  MainMemory mem;
  MemoryUtils::LoadImage(mem, bin_image);
  MemoryUtils::CoreDump(mem);

  return 0;
}

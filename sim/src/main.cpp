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

  const std::string image_name = FLAGS_riscv_binary;
  VLOG(1) << "Executing program: " << image_name << std::endl;
  MemoryPtr mem = std::make_shared<MainMemory>(MainMemory(image_name));

  return 0;
}

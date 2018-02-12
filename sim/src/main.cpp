#include <glog/logging.h>
#include <gflags/gflags.h>

#include <iostream>

#include <register_file.hpp>

DEFINE_string(riscv_binary, "my_asm", "Program to run in simulator");

int main(int argc, char* argv[]) {
  google::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);

  LOG(INFO) << "Executing program: " << FLAGS_riscv_binary << std::endl;

  return 0;
}

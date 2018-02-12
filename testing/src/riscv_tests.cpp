#include <iostream>

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <gtest/gtest.h>

#include <register_file.hpp>
#include <register_file_tests.hpp>

TEST(register_file_tests, read_write_test) {
  RegisterFile register_file;
  RegisterFileTestClass register_file_tester;
  LOG(INFO) << "Running read write tests!";
}

int main(int argc, char* argv[]) {
  google::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

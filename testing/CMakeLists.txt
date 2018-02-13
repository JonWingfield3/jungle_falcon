cmake_minimum_required(VERSION 3.2)
project("testing")

find_package(GTest REQUIRED)
find_package(glog 0.3.5 REQUIRED)
find_package(gflags REQUIRED)

set(RISCV_SIM_DIR ../sim)
set(RISCV_ASM_DIR ../asm)

set(CMAKE_CXX_FLAGS "--std=c++11 ${CMAKE_CXX_FLAGS}")
set(TESTING_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(TESTING_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

set(SIM_INCLUDE_DIR ${RISCV_SIM_DIR}/include)
set(SIM_SOURCE_DIR ${RISCV_SIM_DIR}/src)

include_directories(
  ${TESTING_INCLUDE_DIR}
  ${PROJ_INCLUDE_DIR}
  ${SIM_INCLUDE_DIR})

set(TESTING_HEADERS
  ${TESTING_INCLUDE_DIR}/register_file_tests.hpp
  ${SIM_INCLUDE_DIR}/register_file.hpp)

set(TESTING_SOURCES
  ${TESTING_SOURCE_DIR}/register_file_tests.cpp
  ${SIM_SOURCE_DIR}/register_file.cpp)

add_executable(riscv_tests
  ${TESTING_SOURCE_DIR}/riscv_tests.cpp
  ${TESTING_HEADERS}
  ${TESTING_SOURCES})
target_link_libraries(riscv_tests 
  gtest
  glog::glog
  gflags
  pthread)

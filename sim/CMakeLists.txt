cmake_minimum_required (VERSION 3.2)
project ("riscv_sim")

find_package(glog 0.3.5 REQUIRED)
find_package(gflags REQUIRED)

find_path(gflags_INCLUDE_DIR NAMES gflags)

set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

set(BUILD_TARGET "INSTRUCTION_ACCURATE" CACHE STRING "Default build is IA")
set_property(CACHE BUILD_TARGET PROPERTY STRINGS CYCLE_ACCURATE INSTRUCTION_ACCURATE)

set(CMAKE_C_FLAGS "--std=c99 -g -O1 ${CMAKE_C_FLAGS}")
set(CMAKE_CXX_FLAGS "--std=c++11 ${CMAKE_CXX_FLAGS}")

list(APPEND LINK_FLAGS "")

list(APPEND SRC_FILES
  ${SOURCE_DIR}/register_file.cpp
)

list(APPEND HEADER_FILES
  ${INCLUDE_DIR}/register_file.hpp
)

include_directories(${INCLUDE_DIR}
  ${glog_INCLUDE_DIR}
  ${flags_INCLUDE_DIR})

if(BUILD_TARGET STREQUAL "CYCLE_ACCURATE")
  add_executable(riscv_sim
    ${SOURCE_DIR}/main.cpp
    ${SRC_FILES}
    ${HEADER_FILES})
  target_compile_definitions(riscv_sim PUBLIC -D__CYCLE_ACCURATE__=1)
  target_link_libraries(riscv_sim
    glog::glog
    gflags)
elseif(BUILD_TARGET STREQUAL "INSTRUCTION_ACCURATE")
  add_executable(riscv_sim
    ${SOURCE_DIR}/main.cpp
    ${SRC_FILES}
    ${HEADER_FILES})
  target_compile_definitions(riscv_sim PUBLIC -D__INSTRUCTION_ACCURATE__=1)
  target_link_libraries(riscv_sim glog::glog gflags)
endif()

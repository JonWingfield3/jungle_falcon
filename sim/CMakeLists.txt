cmake_minimum_required (VERSION 3.2)
project ("riscv_sim")

find_package(glog 0.3.5 REQUIRED)
find_package(gflags REQUIRED)

find_path(gflags_INCLUDE_DIR NAMES gflags)

set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

set(BUILD_TARGET "INSTRUCTION_ACCURATE" CACHE STRING "Default build is IA")
set(CACHE_OPTION "ENABLE_CACHE" CACHE STRING "Default is no cache memory")
set_property(CACHE BUILD_TARGET PROPERTY STRINGS CYCLE_ACCURATE INSTRUCTION_ACCURATE)
set_property(CACHE CACHE_OPTION PROPERTY STRINGS ENABLE_CACHE DISABLE_CACHE)

set(CMAKE_C_FLAGS "--std=c99 -g -O1 ${CMAKE_C_FLAGS}")
set(CMAKE_CXX_FLAGS "--std=c++14 ${CMAKE_CXX_FLAGS}")

list(APPEND LINK_FLAGS "")

list(APPEND SRC_FILES
  ${SOURCE_DIR}/alu.cpp
  ${SOURCE_DIR}/commands.cpp
  ${SOURCE_DIR}/command_interpreter.cpp
  ${SOURCE_DIR}/cpu.cpp
  ${SOURCE_DIR}/instruction_factory.cpp
  ${SOURCE_DIR}/memory.cpp
  ${SOURCE_DIR}/pipeline.cpp
  ${SOURCE_DIR}/register_file.cpp)

list(APPEND HEADER_FILES
  ${INCLUDE_DIR}/alu.hpp
  ${INCLUDE_DIR}/b_type_instructions.hpp
  ${INCLUDE_DIR}/commands.hpp
  ${INCLUDE_DIR}/command_interpreter.hpp
  ${INCLUDE_DIR}/cpu.hpp
  ${INCLUDE_DIR}/instructions.hpp
  ${INCLUDE_DIR}/instruction_factory.hpp
  ${INCLUDE_DIR}/i_type_instructions.hpp
  ${INCLUDE_DIR}/j_type_instructions.hpp
  ${INCLUDE_DIR}/memory.hpp
  ${INCLUDE_DIR}/pipeline.hpp
  ${INCLUDE_DIR}/register_file.hpp
  ${INCLUDE_DIR}/riscv_defs.hpp
  ${INCLUDE_DIR}/r_type_instructions.hpp
  ${INCLUDE_DIR}/s_type_instructions.hpp
  ${INCLUDE_DIR}/u_type_instructions.hpp)

include_directories(${INCLUDE_DIR}
  ${glog_INCLUDE_DIR}
  ${flags_INCLUDE_DIR})

add_executable(riscv_sim
  ${SOURCE_DIR}/main.cpp
  ${SRC_FILES}
  ${HEADER_FILES})
  
if (BUILD_TARGET STREQUAL "CYCLE_ACCURATE")
  target_compile_definitions(riscv_sim PUBLIC -D__CYCLE_ACCURATE__=1)
  target_compile_definitions(riscv_sim PUBLIC -D__INSTRUCTION_ACCURATE__=0)
elseif (BUILD_TARGET STREQUAL "INSTRUCTION_ACCURATE")
  target_compile_definitions(riscv_sim PUBLIC -D__CYCLE_ACCURATE__=0)
  target_compile_definitions(riscv_sim PUBLIC -D__INSTRUCTION_ACCURATE__=1)
endif()

if (CACHE_OPTION STREQUAL "ENABLE_CACHE")
  target_compile_definitions(riscv_sim PUBLIC -D__CACHE__=1)
elseif (CACHE_OPTION STREQUAL "DISABLE_CACHE")
  target_compile_definitions(riscv_sim PUBLIC -D__CACHE__=0)
endif()

target_link_libraries(riscv_sim glog::glog gflags)

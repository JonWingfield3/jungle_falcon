cmake_minimum_required(VERSION 3.2)
project("asm")

add_custom_target(asm ALL 
  COMMAND make all
  COMMAND cp build/* ../build/asm/
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

cmake_minimum_required(VERSION 3.2)
project("c")

add_custom_target(c ALL 
  COMMAND make all
  COMMAND cp build/* ../build/c/
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

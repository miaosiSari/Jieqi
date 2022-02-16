cmake_minimum_required(VERSION 3.15)

project(cppjieqi)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-O3 -g -Wall -Wextra")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
aux_source_directory(global/ DIR_SRCS)
aux_source_directory(score/ DIR_SRCS)
aux_source_directory(board/ DIR_SRCS)
aux_source_directory(. DIR_SRCS)
add_executable(cppjieqi ${DIR_SRCS})


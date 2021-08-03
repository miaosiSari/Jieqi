cmake_minimum_required(VERSION 3.15)

project(cppjieqi)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-O2 -Wall -Wextra")
add_link_options(-rdynamic)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
aux_source_directory(. DIR_SRCS)
aux_source_directory(global/ DIR_SRCS)
aux_source_directory(log/ DIR_SRCS)
aux_source_directory(score/ DIR_SRCS)
aux_source_directory(board/ DIR_SRCS)
add_executable(cppjieqi ${DIR_SRCS})
#import qt
FIND_PACKAGE(Qt5Core)

TARGET_LINK_LIBRARIES(cppjieqi Qt5::Core)


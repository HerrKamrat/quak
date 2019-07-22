cmake_minimum_required(VERSION 3.0)

project(catch)

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/catch")

set(CMAKE_CXX_STANDARD 17)

# Prepare "Catch" library for other executables
add_library(catch INTERFACE)
target_include_directories(catch INTERFACE ${SRC_DIR})

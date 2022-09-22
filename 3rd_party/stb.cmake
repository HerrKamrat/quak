cmake_minimum_required(VERSION 3.0)

project(stb)

set(CMAKE_CXX_STANDARD 17)

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/stb/")

add_library(stb INTERFACE)
add_library(stb-header-only ALIAS stb)

target_include_directories(stb INTERFACE ${SRC_DIR})

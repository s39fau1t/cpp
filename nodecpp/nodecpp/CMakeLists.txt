cmake_minimum_required(VERSION 3.26.4)

project(nodecpp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS False)

set(CMAKE_EXPORT_COMPILE_COMMANDS True)

add_library(nodecpp STATIC src/nodecpp.cpp src/fs.cpp)

target_compile_options(nodecpp PRIVATE -Wall -Wextra -Wpedantic)

target_include_directories(nodecpp PRIVATE "include" "libraries/include")

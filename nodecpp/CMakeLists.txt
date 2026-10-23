cmake_minimum_required(VERSION 3.26.4)

project(main)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS False)

set(CMAKE_EXPORT_COMPILE_COMMANDS True)

add_subdirectory(nodecpp)

add_executable(main main.cpp)

target_compile_options(main PRIVATE -Wall -Wextra -Wpedantic)

target_include_directories(main PRIVATE "${PROJECT_SOURCE_DIR}/nodecpp/include")
target_link_directories(main PRIVATE "${PROJECT_BINARY_DIR}/nodecpp" "${PROJECT_SOURCE_DIR}/libraries/lib")

target_link_libraries(main uv)
target_link_libraries(main nodecpp)

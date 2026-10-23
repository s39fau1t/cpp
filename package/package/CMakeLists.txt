cmake_minimum_required(VERSION 3.28)

project(package)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS False)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_EXPORT_COMPILE_COMMANDS True)

add_executable(package src/main.cpp)

target_sources(package PRIVATE
	FILE_SET cxx_modules TYPE CXX_MODULES
	FILES
		src/commands/build.cpp
		src/commands/compile.cpp
		src/commands/config.cpp
		src/commands/error.cpp
		src/commands/get_config.cpp
		src/commands/new.cpp
		src/commands.cpp
)

target_include_directories(package PRIVATE libraries/include)

target_compile_options(package PRIVATE -Wall -Wextra -Wpedantic)


cmake_minimum_required(VERSION 3.29 FATAL_ERROR)

project(Tester)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

include(CMakePrintHelpers)

# find_package(SDL2 CONFIG REQUIRED)

# cmake_print_variables(SDL2_LIBRARIES)
add_executable(main src/main.cxx)

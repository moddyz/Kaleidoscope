cmake_minimum_required(VERSION 3.10)

# Project Name
project(
    Kaleidoscope VERSION 0.0
    DESCRIPTION "Kaleidoscope Language"
    LANGUAGES CXX
)

# Build with C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Compiler options
add_compile_options(-g -O3)

# Build kaleidoscope library.
add_subdirectory(kaleidoscope)

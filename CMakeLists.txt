cmake_minimum_required(VERSION 3.10)

# Project Name
project(
    Kaleidoscope VERSION 0.0
    DESCRIPTION "Kaleidoscope Language"
    LANGUAGES CXX
)

# Build with C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Compiler options
add_compile_options(-g -O3)

# Build kaleidoscope targets.
add_subdirectory(kaleidoscope)
add_subdirectory(kaleidoscopeParser)

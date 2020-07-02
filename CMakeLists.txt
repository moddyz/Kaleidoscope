cmake_minimum_required(VERSION 3.17)

# Project Name
project(
    Kaleidoscope
    VERSION 0.0
    DESCRIPTION "Kaleidoscope Language."
    LANGUAGES CXX
)

# Options
option(BUILD_DOCUMENTATION "Build doxygen documentation." ON)

# Cross-platform compatibility.
if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

# Load CMake modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
                      ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules
                      ${CMAKE_CURRENT_SOURCE_DIR}/cmake/macros)
include(Public)
set(LLVM_STATIC TRUE)
find_package(LLVM REQUIRED)

if(BUILD_DOCUMENTATION)
    file(
        COPY ${CMAKE_SOURCE_DIR}/README.md
        DESTINATION ${CMAKE_BINARY_DIR}
    )

    doxygen_documentation(
        doxygen_docs
        GENERATE_TAGFILE
            TRUE
        DOXYFILE
            ${CMAKE_SOURCE_DIR}/docs/Doxyfile.in
        INPUTS
            ${CMAKE_CURRENT_SOURCE_DIR}/src/
            ${CMAKE_CURRENT_SOURCE_DIR}/src/kaleidoscope/
    )
endif()

# Build targets.
add_subdirectory(src)

# Install exported targets.
export_project(${CMAKE_SOURCE_DIR}/cmake/Config.cmake.in)

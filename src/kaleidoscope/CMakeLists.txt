set(LIBRARY_NAME "kaleidoscope")

file(GLOB PUBLIC_HEADERS *.h)
file(GLOB CPPFILES *.cpp)

cpp_library(${LIBRARY_NAME}
    TYPE
        SHARED
    CPPFILES
        ${CPPFILES}
    PUBLIC_HEADERS
        ${PUBLIC_HEADERS}
    INCLUDE_PATHS
        ${LLVM_INCLUDES}
    LIBRARIES
        ${LLVM_LIBRARIES}
        ${LLVM_SYSTEM_LIBRARIES}
)

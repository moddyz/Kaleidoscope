set(PROGRAM_NAME "kaleidoscopeInterpreter")

cpp_program(${PROGRAM_NAME}
    LIBRARIES
        kaleidoscope
    CPPFILES
        main.cpp
    INCLUDE_PATHS
        ${LLVM_INCLUDES}
    LIBRARIES
        ${LLVM_LIBRARIES}
        ${LLVM_SYSTEM_LIBRARIES}
)

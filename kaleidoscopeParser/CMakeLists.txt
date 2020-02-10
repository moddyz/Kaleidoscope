set(PACKAGE_NAME "kaleidoscopeParser")

# Build executable
file(GLOB SRC_FILES *.cpp)
add_executable(${PACKAGE_NAME} ${SRC_FILES})
target_include_directories(
    ${PACKAGE_NAME} PUBLIC ${CMAKE_BINARY_DIR}/include
)
target_link_libraries(
    ${PACKAGE_NAME} PUBLIC kaleidoscope
)

# Install
install(
    TARGETS ${PACKAGE_NAME}
    DESTINATION bin
)

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

if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

if (LINUX)
    set_target_properties(${PACKAGE_NAME} PROPERTIES LINK_FLAGS "-Wl,-rpath,$ORIGIN/../lib/")
endif()

if (APPLE)
    set_target_properties(${PACKAGE_NAME} PROPERTIES LINK_FLAGS "-Wl,-rpath,@executable_path/../lib/")
endif()

# Install
install(
    TARGETS ${PACKAGE_NAME}
    DESTINATION bin
)

set(PACKAGE_NAME "kaleidoscope")

# Public headers
file(GLOB PUBLIC_HEADERS *.h)
file(
    COPY ${PUBLIC_HEADERS} 
    DESTINATION ${CMAKE_BINARY_DIR}/include/${PACKAGE_NAME}
)
install(
    FILES ${PUBLIC_HEADERS}    
    DESTINATION ${CMAKE_INSTALL_PREFIX}/include/${PACKAGE_NAME}
)

# Library.
file(GLOB SRC_FILES *.cpp)
add_library(${PACKAGE_NAME} STATIC ${SRC_FILES})
target_include_directories(
    ${PACKAGE_NAME} PUBLIC ${CMAKE_BINARY_DIR}/include
)
install(
    TARGETS ${PACKAGE_NAME}
    DESTINATION lib
)

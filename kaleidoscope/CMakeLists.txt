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
add_library(${PACKAGE_NAME} SHARED ${SRC_FILES})
target_include_directories(
    ${PACKAGE_NAME} PUBLIC ${CMAKE_BINARY_DIR}/include ${LLVM_INCLUDES}
)
target_link_libraries(
    ${PACKAGE_NAME} PUBLIC ${LLVM_LIBRARIES} ${LLVM_SYSTEM_LIBRARIES}
)

install(
    TARGETS ${PACKAGE_NAME}
    DESTINATION lib
)

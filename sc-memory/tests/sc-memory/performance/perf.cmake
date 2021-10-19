file (GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/*.hpp")

add_executable(sc-memory-performance-test ${SOURCES})

target_link_libraries(sc-memory-performance-test PUBLIC sc-memory benchmark::benchmark)

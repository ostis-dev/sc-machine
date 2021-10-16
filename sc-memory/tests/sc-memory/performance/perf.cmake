file (GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/*.hpp")

set (TARGET_NAME sc-memory-performance-test)

add_executable(${TARGET_NAME} ${SOURCES})

target_link_libraries(${TARGET_NAME} PUBLIC sc-memory benchmark::benchmark)

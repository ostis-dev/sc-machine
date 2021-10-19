set(ANTLR_JAR "${CMAKE_CURRENT_SOURCE_DIR}/antlr/antlr-4.7.1-complete.jar" CACHE STRING "ANTLR jar file")

add_subdirectory(antlr/runtime)
add_library(sc_machine_antlr INTERFACE)
target_link_libraries(sc_machine_antlr INTERFACE antlr4_static)
target_include_directories(sc_machine_antlr INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/antlr/runtime/src")

if (BUILD_TESTING)
    add_subdirectory(gtest)

    set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Disable google tests" FORCE)
    set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "Disable google tests" FORCE)
    add_subdirectory(benchmark)
endif ()

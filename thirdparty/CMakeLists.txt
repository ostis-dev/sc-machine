# Build antlr4 runtime
add_subdirectory(antlr/runtime)

if(BUILD_TESTING)
    include(gtest.cmake)
    include(benchmark.cmake)
endif()

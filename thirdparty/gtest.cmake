add_library(gtest STATIC "gtest/googletest/src/gtest-all.cc" "gtest/googlemock/src/gmock-all.cc"
                         "gtest/googletest/src/gtest_main.cc")

target_include_directories(
    gtest
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/gtest/googletest
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/gtest/googletest/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/gtest/googlemock
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/gtest/googlemock/include)


add_subdirectory(sc-core)
add_subdirectory(sc-memory)

if (BUILD_TESTING)
  add_subdirectory(tests)
endif ()


file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp" "src/*.h" "src/*.hpp")

add_executable(sc-builder ${SOURCES})
target_include_directories(sc-builder PRIVATE ${SC_MEMORY_SRC})
target_link_libraries(sc-builder 
  PUBLIC sc-core sc-memory
  PRIVATE Boost::filesystem Boost::program_options Boost::system PkgConfig::glib)

add_dependencies(sc-builder sc-code-generator sc-kpm sc-memory)

sc_codegen(sc-builder ${CMAKE_CURRENT_SOURCE_DIR}/src)

if (BUILD_TESTING)
  include (tests/tests.cmake)
endif()
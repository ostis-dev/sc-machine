set (SC_SERVER_DIR "${SC_MACHINE_ROOT}/sc-server")

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp" "src/*.hpp")

add_executable(sc-server ${SOURCES})
target_include_directories(sc-server PRIVATE ${SC_MEMORY_SRC})
target_link_libraries(sc-server sc-memory Boost::program_options PkgConfig::glib)

add_dependencies(sc-server
  sc-kpm
  sc-memory
)

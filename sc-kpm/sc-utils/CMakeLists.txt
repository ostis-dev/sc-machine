file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "*.c" "*.h")

add_library (sc-utils SHARED ${SOURCES})

target_include_directories(sc-utils PRIVATE ${SC_MEMORY_SRC})
add_dependencies(sc-utils sc-core sc-kpm-common)
target_link_libraries(sc-utils sc-kpm-common PkgConfig::glib)

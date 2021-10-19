set(SC_CORE_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "*.c" "*.cpp" "*.h" "*.hpp")

add_library (sc-core SHARED ${SOURCES})

target_link_libraries(sc-core sc_machine_rocksdb PkgConfig::glib PkgConfig::gmodule)
target_compile_definitions(sc-core PRIVATE "-DSC_MEMORY_SELF_BUILD")

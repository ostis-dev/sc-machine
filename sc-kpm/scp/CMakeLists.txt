
set(SCP_ROOT ${CMAKE_CURRENT_LIST_DIR})

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "*.c" "*.cpp" "*.h")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${SC_MACHINE_ROOT}/bin/extensions)

set(CMAKE_C_FLAGS_DEBUG "-DSCP_DEBUG")

add_definitions(-DSCP_MATH)
add_definitions(-DSCP_STRING)

add_library (scp SHARED ${SOURCES})

target_include_directories(scp 
  PRIVATE "${SCP_ROOT}/scp_lib"
  PRIVATE ${SC_MEMORY_SRC}
  )
add_dependencies(scp sc-core)

target_link_libraries(scp 
  PUBLIC sc-core 
  PRIVATE PkgConfig::glib)

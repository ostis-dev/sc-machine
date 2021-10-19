set (SC_SERVER_DIR "${SC_MACHINE_ROOT}/sc-server")

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp" "src/*.hpp")

if (WIN32)
  set (BOOST_LIBS_LIST )
elseif (APPLE)
  set (BOOST_LIBS_LIST
    Boost::program_options
    )
else()
  set (BOOST_LIBS_LIST
    boost_program_options
    )
endif ()
  
add_executable(sc-server ${SOURCES})
target_include_directories(sc-server 
  PRIVATE ${SC_MEMORY_SRC} 
  PRIVATE ${GLIB2_INCLUDE_DIRS}
  )
target_link_libraries(sc-server sc-memory ${BOOST_LIBS_LIST})

add_dependencies(sc-server
  sc-kpm
  sc-memory
)

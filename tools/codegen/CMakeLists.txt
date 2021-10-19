set (SC_CODEGEN_DIR "${SC_TOOLS_DIR}/codegen")

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "Parser/*.cpp" "Parser/*.hpp")

add_executable(sc-code-generator ${SOURCES})
target_include_directories(sc-code-generator PRIVATE ${SC_CODEGEN_DIR}/Parser)

if(APPLE)
  set (BOOST_LIBS_LIST
       Boost::filesystem
       Boost::program_options
      )
elseif(UNIX)
  set (BOOST_LIBS_LIST
       boost_filesystem
       boost_program_options
       boost_system
      )
endif()
  
set_target_properties(sc-code-generator PROPERTIES COMPILE_FLAGS ${LIBCLANG_CXXFLAGS})

target_link_libraries(sc-code-generator ${LIBCLANG_LIBRARY} ${BOOST_LIBS_LIST})

set (SC_CODEGEN_DIR "${SC_TOOLS_DIR}/codegen")

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "Parser/*.cpp" "Parser/*.hpp")

add_executable(sc-code-generator ${SOURCES})
target_include_directories(sc-code-generator PRIVATE ${SC_CODEGEN_DIR}/Parser)

target_link_libraries(sc-code-generator sc_machine_libclang Boost::filesystem Boost::program_options Boost::system)

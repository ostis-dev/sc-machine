
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "*.cpp" "*.hpp")
add_executable(sctp-client-test ${SOURCES})
target_link_libraries(sctp-client-test sctp-client)


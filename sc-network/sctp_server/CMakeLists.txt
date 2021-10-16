
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "*.cpp" "*.h")

add_executable(sctp-server ${SOURCES})

target_include_directories(sctp-server
	PRIVATE ${SC_MEMORY_SRC} 
	PRIVATE ${GLIB2_INCLUDE_DIRS}
	)

target_link_libraries(sctp-server sc-memory
                                  Qt5::Core
                                  Qt5::Network)

add_dependencies(sctp-server
	sc-kpm
	)
set(SCTP_CLIENT_ROOT "${SC_MACHINE_ROOT}/sc-network/sctp_client")

file(GLOB SOURCES CONFIGURE_DEPENDS "*.cpp" "*.hpp")

if (WIN32)
    set (SOURCES ${SOURCES} "sockets/winSocket.cpp" "sockets/winSocket.hpp")
    set (SOCKET_LIBS )
else ()
    set (SOURCES ${SOURCES} "sockets/glibSocket.cpp" "sockets/glibSocket.hpp")
    set (SOCKET_LIBS  ${GLIB2_LIBRARIES})
endif()

add_library (sctp-client SHARED ${SOURCES})

target_include_directories(sctp-client
  PRIVATE ${SC_MEMORY_SRC}
  PRIVATE ${GLIB2_INCLUDE_DIRS}
  )
target_link_libraries(sctp-client sc-memory ${SOCKET_LIBS})
target_compile_definitions(sctp-client PUBLIC "-DSC_MEMORY_SELF_BUILD")

if (WIN32)
add_subdirectory(test)
endif ()

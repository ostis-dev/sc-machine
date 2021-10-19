add_custom_command(OUTPUT "${CMAKE_CURRENT_LIST_DIR}/scs/scsLexer.cpp"
                          "${CMAKE_CURRENT_LIST_DIR}/scs/scsLexer.h"
                          "${CMAKE_CURRENT_LIST_DIR}/scs/scsParser.cpp"
                          "${CMAKE_CURRENT_LIST_DIR}/scs/scsParser.h"
      COMMAND "${Java_JAVA_EXECUTABLE}" -jar "${ANTLR_JAR}" scs.g4 -no-listener -package scs
      WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/scs"
      DEPENDS scs/scs.g4
)

add_custom_target(SCsParser ALL
      DEPENDS "${CMAKE_CURRENT_LIST_DIR}"
      SOURCES "scs/scs.g4"
)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "*.cpp" "*.hpp")

set (SOURCES ${SOURCES}
      "${CMAKE_CURRENT_LIST_DIR}/scs/scsLexer.cpp"
      "${CMAKE_CURRENT_LIST_DIR}/scs/scsLexer.h"
      "${CMAKE_CURRENT_LIST_DIR}/scs/scsParser.cpp"
      "${CMAKE_CURRENT_LIST_DIR}/scs/scsParser.h"
      )

add_library (sc-memory SHARED ${SOURCES})

target_include_directories(sc-memory PUBLIC ${SC_MEMORY_SRC})

target_link_libraries(sc-memory 
      PUBLIC sc-core 
      PRIVATE sc_machine_antlr curl Boost::program_options Boost::filesystem Boost::system PkgConfig::glib)

sc_codegen_ex(sc-memory ${CMAKE_CURRENT_SOURCE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/generated")
add_dependencies(sc-memory 
  sc-core
  sc-code-generator
  sc_machine_antlr
  SCsParser)

target_compile_definitions(sc-memory PRIVATE "-DSC_MEMORY_SELF_BUILD")

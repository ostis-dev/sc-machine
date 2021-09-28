make_tests_from_folder(${CMAKE_CURRENT_LIST_DIR}/common
                       NAME sc-memory-common-tests
                       DEPENDS sc-memory
                       INCLUDES ${SC_MEMORY_SRC} ${CMAKE_CURRENT_LIST_DIR}/_test)

sc_codegen(sc-memory-common-tests ${CMAKE_CURRENT_LIST_DIR}/common)

make_tests_from_folder(${CMAKE_CURRENT_LIST_DIR}/agents
                       NAME sc-memory-agents-tests
                       DEPENDS sc-memory
                       INCLUDES ${SC_MEMORY_SRC} ${CMAKE_CURRENT_LIST_DIR}/_test)

sc_codegen(sc-memory-agents-tests ${CMAKE_CURRENT_LIST_DIR}/agents)

make_tests_from_folder(${CMAKE_CURRENT_LIST_DIR}/events
                       NAME sc-memory-events-tests
                       DEPENDS sc-memory
                       INCLUDES ${SC_MEMORY_SRC} ${CMAKE_CURRENT_LIST_DIR}/_test)

make_tests_from_folder(${CMAKE_CURRENT_LIST_DIR}/scs
                       NAME sc-memory-scs-tests
                       DEPENDS sc-memory
                       INCLUDES ${SC_MEMORY_SRC} ${CMAKE_CURRENT_LIST_DIR}/_test)

make_tests_from_folder(${CMAKE_CURRENT_LIST_DIR}/templates
                       NAME sc-memory-template-tests
                       DEPENDS sc-memory
                       INCLUDES ${SC_MEMORY_SRC} ${CMAKE_CURRENT_LIST_DIR}/_test)

make_tests_from_folder(${CMAKE_CURRENT_LIST_DIR}/codegen
                       NAME sc-memory-codegen-tests
                       DEPENDS sc-memory
                       INCLUDES ${SC_MEMORY_SRC} ${CMAKE_CURRENT_LIST_DIR}/_test)

sc_codegen(sc-memory-codegen-tests ${CMAKE_CURRENT_LIST_DIR}/codegen)

include(${CMAKE_CURRENT_LIST_DIR}/performance/perf.cmake)


set(SC_COMMON_SRC ${CMAKE_CURRENT_LIST_DIR})

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "*.c" "*.h")

add_library(sc-kpm-common SHARED ${SOURCES})

target_include_directories(sc-kpm-common
	PRIVATE ${SC_COMMON_SRC}
	PRIVATE ${SC_MEMORY_SRC}
	)

add_dependencies(sc-kpm-common sc-core)
target_link_libraries(sc-kpm-common 
	PUBLIC sc-core 
	PRIVATE PkgConfig::glib)
target_compile_definitions(sc-kpm-common PUBLIC "-DSC_KPM_COMMON_SELF_BUILD")

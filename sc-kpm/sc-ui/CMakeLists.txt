file(GLOB_RECURSE SOURCES "*.cpp" "*.h")

add_library (sc-ui SHARED ${SOURCES} translators/uiSc2uiJsonTranslator.cpp translators/uiSc2uiJsonTranslator.h)
SET(CMAKE_BUILD_TYPE Debug)
target_include_directories(sc-ui 
	PRIVATE ${SC_MEMORY_SRC}
	PRIVATE ${GLIB2_INCLUDE_DIRS}
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
	)
add_dependencies(sc-ui sc-core sc-kpm-common)
target_link_libraries(sc-ui sc-kpm-common)

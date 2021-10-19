set(SC_KPM_ROOT "${SC_MACHINE_ROOT}/sc-kpm")

add_subdirectory(sc-common)

file(MAKE_DIRECTORY ${SC_EXTENSION_DIRECTORY})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${SC_EXTENSION_DIRECTORY})

add_subdirectory(sc-ui)
add_subdirectory(sc-search)
add_subdirectory(sc-utils)

add_custom_target(sc-kpm)

if (SC_KPM_SCP)
    add_subdirectory(scp)
    add_dependencies(sc-kpm scp)
endif()

add_dependencies(sc-kpm
    sc-kpm-common
    sc-ui
    sc-search
    sc-utils
)

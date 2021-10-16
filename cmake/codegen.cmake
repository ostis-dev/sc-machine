macro(sc_codegen_ex Target SrcPath OutputPath)
    # fetch all include directories for the project target
    get_target_property(DIRECTORIES ${Target} INCLUDE_DIRECTORIES)

    # build the include directory flags
    foreach (DIRECTORY ${DIRECTORIES})
        set(META_FLAGS ${META_FLAGS} "-I${DIRECTORY}")
    endforeach ()

    get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
    foreach(dir ${dirs})
          set(META_FLAGS ${META_FLAGS} "-I${dir}")
    endforeach()

    # include the system directories
    if (MSVC)
        # assume ${VS_VERSION} is the version of Visual Studio being used to compile
        set(META_FLAGS ${META_FLAGS} 
            "-IC:/Program Files (x86)/Microsoft Visual Studio ${VS_VERSION}/VC/include"
        )
    elseif (UNIX)
        set(META_FLAGS ${META_FLAGS}
            "-I${LIBCLANG_LIBDIR}/clang/${LIBCLANG_VERSION_STRING}/include/"
        )
    else ()
        # you can figure it out for other compilers :)
        message(FATAL_ERROR "System include directories not implemented for this compiler.")
    endif ()

    if (MSVC)
        add_custom_command(
            PRE_BUILD
            TARGET ${Target}
            COMMAND call "${SC_CODEGEN_TOOL}"
            --target     "${Target}"
            --source     "${SrcPath}"
            --output     "${OutputPath}"
            --flags      "${META_FLAGS}"
            --build_dir  "${CMAKE_CURRENT_BINARY_DIR}"
            --cache
        )
    else()
        file(GLOB_RECURSE HEADER_FILES "${SrcPath}/*.hpp")

        set (CACHE_FILE "${CMAKE_CURRENT_BINARY_DIR}/${Target}.gen_cache")
        add_custom_command(
            OUTPUT ${CACHE_FILE}
            COMMAND "${SC_CODEGEN_TOOL}"
            --target     "${Target}"
            --source     "${SrcPath}"
            --output     "${OutputPath}"
            --build_dir  "${CMAKE_CURRENT_BINARY_DIR}"
            --flags      "'${META_FLAGS}'"
            --cache
            DEPENDS ${HEADER_FILES}
        )

        set (SUB_TARGET "${Target}-GenerateCode")
        add_custom_target(${SUB_TARGET} DEPENDS ${CACHE_FILE} SOURCES ${HEADER_FILES})

        add_dependencies(${SUB_TARGET} sc-code-generator)
        add_dependencies(${Target} ${SUB_TARGET})
    endif()

    target_include_directories(${Target} PUBLIC ${OutputPath})

    # clean stage
    set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES CACHE_FILE)

endmacro()

macro(sc_codegen Target SrcPath)
    sc_codegen_ex(${Target} ${SrcPath} "${CMAKE_CURRENT_BINARY_DIR}/${Target}_gen")
endmacro()

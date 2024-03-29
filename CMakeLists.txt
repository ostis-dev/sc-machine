project (sc-machine)
site_name (www.ostis.net)
cmake_minimum_required (VERSION 2.8)

include (CTest)
include (CheckCCompilerFlag)
include (CMakeDependentOption)

set (SC_BIN_PATH "${CMAKE_CURRENT_LIST_DIR}/bin")

set (SC_MACHINE_ROOT ${CMAKE_CURRENT_LIST_DIR})
set (SC_MACHINE_THIRDPARTY_PATH "${SC_MACHINE_ROOT}/thirdparty")

option (SC_AUTO_TEST "Flag to build for automation testing" OFF)
option (SC_KPM_SCP "Flag to build SCP module" OFF)
option (SC_BUILD_SCTP "Flag to turn on/off sctp protocol support" OFF)
option (SC_BUILD_ARM64 "Flag to build arm64" OFF)

set(SC_USE_SANITIZER "none" CACHE STRING "Build with specified sanitizer")
set_property(CACHE SC_USE_SANITIZER PROPERTY STRINGS none address memory)

message ("Sanitizer: ${SC_USE_SANITIZER}")

cmake_dependent_option(SC_COVERAGE "Flag to generate coverage reports" OFF "SC_USE_SANITIZER STREQUAL none;ON" OFF)

# Coverage
if(SC_COVERAGE)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage -O0 -g")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -O0 -g")
endif()

if (${SC_BUILD_ARM64})
    check_c_compiler_flag("-arch arm64" IS_ARM64_SUPPORTED)
    if (${IS_ARM64_SUPPORTED})
        set (CMAKE_OSX_ARCHITECTURES "arm64" CACHE STRING "Used architecture" FORCE)
        message ("-- Architecture: arm64")
    else()
        message (FATAL_ERROR "Arm64 platform is not supported by compiler")
    endif()
endif()

# codegen
if (${UNIX})
    set(SC_CODEGEN_TOOL "${SC_BIN_PATH}/sc-code-generator")
elseif (${WIN32})
    set(SC_CODEGEN_TOOL "${SC_BIN_PATH}/sc-code-generator.exe")
endif()
include("${SC_MACHINE_ROOT}/cmake/codegen.cmake")

find_package(Java)

set(CMAKE_MODULE_PATH "${SC_MACHINE_ROOT}/cmake")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${SC_BIN_PATH})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${SC_BIN_PATH})

set(SC_EXTENSION_DIRECTORY "${SC_BIN_PATH}/extensions")

# for multi-config builds (e.g. msvc)
foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
    string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${SC_BIN_PATH} )
    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${SC_BIN_PATH} )
endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )

set(SC_MEMORY_SRC "${SC_MACHINE_ROOT}/sc-memory")

if (${SC_USE_SANITIZER} STREQUAL "address")
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -O1 -fno-omit-frame-pointer")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -O1 -fno-omit-frame-pointer")
elseif (${SC_USE_SANITIZER} STREQUAL "memory")
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=memory -fPIE -pie -fno-omit-frame-pointer")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=memory -fPIE -pie -fno-omit-frame-pointer")
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcolor-diagnostics -std=c++17")
    if (${SC_USE_SANITIZER} STREQUAL "none" AND NOT SC_COVERAGE)
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
    endif()

    if(${SC_AUTO_TEST})
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wdeprecated-declarations")
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall")
    if (${SC_USE_SANITIZER} STREQUAL "none" AND NOT SC_COVERAGE)
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")        ## Optimize
        set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -s")  ## Strip binary
    endif()
elseif (NOT CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
endif ()

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DSC_DEBUG -DSC_PROFILE")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DSC_DEBUG -DSC_PROFILE")

set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG")

message ("CMAKE_C_FLAGS: ${CMAKE_C_FLAGS}")
message ("CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")

# find dependencies
if (${APPLE})
    set(Boost_USE_STATIC_LIBS ON)
    find_package(Boost 1.67 REQUIRED COMPONENTS system filesystem program_options)

    find_package(PkgConfig REQUIRED)
    pkg_search_module(GLIB2 REQUIRED glib-2.0)
    pkg_search_module(GLIB2_MODULE REQUIRED gmodule-2.0)

    set (GLIB2_LIBRARIES ${GLIB_LDFLAGS} ${GLIB2_MODULE_LDFLAGS})

    if (NOT DEFINED LIBCLANG_LIBRARIES OR NOT DEFINED LIBCLANG_CXXFLAGS OR NOT DEFINED LIBCLANG_LIBDIR)
        find_package(LibClang REQUIRED)
    endif ()

    find_package(curl REQUIRED)
    find_package(RocksDB REQUIRED)

    set (LIBCURL_LIBRARIES ${CURL_LIBRARIES})

    include_directories("${CMAKE_OSX_SYSROOT}/usr/include")

elseif (${UNIX})
    find_package(RocksDB REQUIRED)

    include(FindPkgConfig)
    pkg_check_modules (GLIB2 REQUIRED glib-2.0)
    pkg_check_modules (GLIB2_MODULE REQUIRED gmodule-2.0)

    set (GLIB2_INCLUDE_DIRS "${GLIB2_INCLUDE_DIRS}" "/usr/lib/x86_64-linux-gnu/glib-2.0/include/" "${GLIB2_MODULE}")
    set (GLIB2_LIBRARIES "${GLIB2_LIBRARIES}" "${GLIB2_MODULE_LIBRARIES}")

    if (NOT DEFINED LIBCLANG_LIBRARIES OR NOT DEFINED LIBCLANG_CXXFLAGS OR NOT DEFINED LIBCLANG_LIBDIR)
        find_package(LibClang REQUIRED)
    endif ()

    pkg_check_modules(LIBCURL REQUIRED libcurl)

    # for std::thread support
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

elseif (${WIN32})

    set(PACKAGES_PATH "${CMAKE_BINARY_DIR}/packages")

    if (${MSVC_VERSION} EQUAL 1800)
        set (MSVC_SUFFIX "vc120")
    elseif (${MSVC_VERSION} EQUAL 1900)
        set (MSVC_SUFFIX "vc140")
    elseif (${MSVC_VERSION} GREATER_EQUAL 1911)
        set (MSVC_SUFFIX "vc141")
    endif()

    set (BOOST_VERSION "1.64.0.0")
    configure_file("${SC_MACHINE_ROOT}/packages.config.in" "${CMAKE_BINARY_DIR}/packages.config")

    file(MAKE_DIRECTORY ${PACKAGES_PATH})
    execute_process(COMMAND "${SC_MACHINE_THIRDPARTY_PATH}/nuget.exe" install ${CMAKE_BINARY_DIR}/packages.config WORKING_DIRECTORY ${PACKAGES_PATH})

    set (GLIB_PATH "${PACKAGES_PATH}/glib.2.36.2.11/build/native")
    set (BOOST_PATH "${PACKAGES_PATH}/boost.${BOOST_VERSION}/lib/native")
    set (ROCKSDB_PATH "${SC_MACHINE_THIRDPARTY_PATH}/rocksdb")

    set (LIBCLANG_INCLUDE_DIRS "${SC_MACHINE_THIRDPARTY_PATH}/clang/include")
    set (LIBCLANG_LIBRARY "libclang")

    set (GLIB2_INCLUDE_DIRS "${GLIB_PATH}/include"
                            "${GLIB_PATH}/include/glib")

    set (LIBCURL_PATH "${SC_MACHINE_THIRDPARTY_PATH}/curl")
    set (LIBCURL_RUNTIME_PATH "${LIBCURL_PATH}/bin")
    set (LIBCURL_INCLUDE_DIRS "${LIBCURL_PATH}/include")
    set (LIBCURL_LIBRARIES "${LIBCURL_PATH}/lib/libcurl.lib" "WSock32.lib" "Ws2_32.lib")
    set (RocksDB_INCLUDE_DIR "${ROCKSDB_PATH}/include")
    set (RocksDB_LIBRARIES "${ROCKSDB_PATH}/rocksdb-shared.lib")

    include_directories("${BOOST_PATH}/include"
                        "${LIBCURL_INCLUDE_DIRS}")

    link_directories(
                     "${SC_MACHINE_THIRDPARTY_PATH}/clang/lib"
                     "${GLIB_PATH}/lib/v110/x64/Release/dynamic/"
                     "${PACKAGES_PATH}/boost_filesystem-${MSVC_SUFFIX}.${BOOST_VERSION}/lib/native/address-model-64/lib/"
                     "${PACKAGES_PATH}/boost_system-${MSVC_SUFFIX}.${BOOST_VERSION}/lib/native/address-model-64/lib/"
                     "${PACKAGES_PATH}/boost_program_options-${MSVC_SUFFIX}.${BOOST_VERSION}/lib/native/address-model-64/lib/"
                     )

    set (GLIB2_LIBRARIES "gio-2.0.lib" "glib-2.0.lib" "gmodule-2.0.lib" "gobject-2.0.lib" "gthread-2.0.lib")

    add_definitions(-DSC_PLATFORM_WIN)

    # copy runtime libraries
    set (GLIB_RUNTIME_PATH "${PACKAGES_PATH}/glib.redist.2.36.2.11/build/native/bin/v110/x64/Release/dynamic")
    set (INTL_RUNTIME_PATH "${PACKAGES_PATH}/libintl.redist.0.18.2.10/build/native/bin/v110/x64/Release/dynamic/cdecl")
    set (ZLIB_RUNTIME_PATH "${PACKAGES_PATH}/zlib.v120.windesktop.msvcstl.dyn.rt-dyn.1.2.8.8/lib/native/v120/windesktop/msvcstl/dyn/rt-dyn/x64/Release")
    set (FFI_RUNTIME_PATH "${PACKAGES_PATH}/libffi.redist.3.0.10.3/build/native/bin/x64/v110/dynamic/Release")
    set (CLANG_RUNTIME_PATH "${SC_MACHINE_THIRDPARTY_PATH}/clang")
    set (WIN_RUNTIME_LIBRARIES
            "${GLIB_RUNTIME_PATH}/glib-2-vs10.dll"
            "${GLIB_RUNTIME_PATH}/gmodule-2-vs10.dll"
            "${GLIB_RUNTIME_PATH}/gio-2-vs10.dll"
            "${GLIB_RUNTIME_PATH}/gobject-2-vs10.dll"
            "${INTL_RUNTIME_PATH}/libintl.dll"
            "${ZLIB_RUNTIME_PATH}/zlib.dll"
            "${FFI_RUNTIME_PATH}/libffi.dll"
            "${CLANG_RUNTIME_PATH}/libclang.dll"
            "${LIBCURL_RUNTIME_PATH}/libcurl.dll"
            "${LIBCURL_RUNTIME_PATH}/libeay32.dll"
            "${LIBCURL_RUNTIME_PATH}/ssleay32.dll"
            "${ROCKSDB_PATH}/rocksdb-shared.dll"
            )
    file(COPY ${WIN_RUNTIME_LIBRARIES} DESTINATION "${SC_BIN_PATH}")

endif(${APPLE})

configure_file(${CMAKE_CURRENT_LIST_DIR}/config/config.ini.in ${CMAKE_CURRENT_LIST_DIR}/bin/config.ini)

if (BUILD_TESTING)
    include (cmake/tests.cmake)
endif()

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/thirdparty)

add_subdirectory(${SC_MACHINE_ROOT}/sc-memory)
add_subdirectory(${SC_MACHINE_ROOT}/sc-kpm)
add_subdirectory(${SC_MACHINE_ROOT}/sc-server)
add_subdirectory(${SC_MACHINE_ROOT}/tools)

if (${SC_BUILD_SCTP})
    add_subdirectory(${SC_MACHINE_ROOT}/sc-network)
endif()

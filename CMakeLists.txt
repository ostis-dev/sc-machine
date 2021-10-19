cmake_minimum_required (VERSION 3.16 FATAL_ERROR)

project (sc-machine)
site_name (www.ostis.net)

include (CTest)
include (CheckCCompilerFlag)
include (CMakeDependentOption)

set (SC_MACHINE_ROOT ${CMAKE_CURRENT_LIST_DIR})
set (SC_MACHINE_THIRDPARTY_PATH "${SC_MACHINE_ROOT}/thirdparty")
set (SC_MEMORY_SRC "${SC_MACHINE_ROOT}/sc-memory")

set (SC_BIN_PATH "${SC_MACHINE_ROOT}/bin")
set (SC_EXTENSION_DIRECTORY "${SC_BIN_PATH}/extensions")

option (SC_AUTO_TEST "Flag to build for automation testing" OFF)
option (SC_KPM_SCP "Flag to build SCP module" OFF)
option (SC_BUILD_SCTP "Flag to turn on/off sctp protocol support" OFF)
option (SC_BUILD_ARM64 "Flag to build arm64" OFF)

set(SC_USE_SANITIZER "none" CACHE STRING "Build with specified sanitizer")
set_property(CACHE SC_USE_SANITIZER PROPERTY STRINGS none address memory)

message ("Sanitizer: ${SC_USE_SANITIZER}")

cmake_dependent_option(SC_COVERAGE "Flag to generate coverage reports" OFF "SC_USE_SANITIZER STREQUAL none;ON" OFF)

# ARM64 support
if (SC_BUILD_ARM64)
    check_c_compiler_flag("-arch arm64" IS_ARM64_SUPPORTED)
    if (IS_ARM64_SUPPORTED)
        set (CMAKE_OSX_ARCHITECTURES "arm64" CACHE STRING "Used architecture" FORCE)
        message (STATUS "Architecture: arm64")
    else()
        message (FATAL_ERROR "Arm64 platform is not supported by compiler")
    endif()
endif()

# Codegen
set(SC_CODEGEN_TOOL "${SC_BIN_PATH}/sc-code-generator")
include("${SC_MACHINE_ROOT}/cmake/codegen.cmake")

# Output directories
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${SC_BIN_PATH})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${SC_BIN_PATH})

# C++17 support
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

# Sanitizers
if (SC_USE_SANITIZER STREQUAL "address")
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -O1 -fno-omit-frame-pointer")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -O1 -fno-omit-frame-pointer")
elseif (SC_USE_SANITIZER STREQUAL "memory")
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=memory -fPIE -pie -fno-omit-frame-pointer")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=memory -fPIE -pie -fno-omit-frame-pointer")
endif()

# Coverage
if(SC_COVERAGE)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage -O0 -g")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -O0 -g")
endif()

# Optimize
if (SC_USE_SANITIZER STREQUAL "none" AND NOT SC_COVERAGE)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
endif()

# Compilers-specific flags
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcolor-diagnostics")
    if(SC_AUTO_TEST)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wdeprecated-declarations")
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
    if (SC_USE_SANITIZER STREQUAL "none" AND NOT SC_COVERAGE)
        # Strip binary
        set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -s")
    endif()
endif ()

# Debug flags
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DSC_DEBUG -DSC_PROFILE")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DSC_DEBUG -DSC_PROFILE")

# Release flags
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG")

message ("CMAKE_C_FLAGS: ${CMAKE_C_FLAGS}")
message ("CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")

# CMake modules
set(CMAKE_MODULE_PATH "${SC_MACHINE_ROOT}/cmake/modules")

# Dependencies
find_package(Java REQUIRED)
find_package(LibClang REQUIRED)
find_package(RocksDB REQUIRED)
find_package(CURL REQUIRED)

# Boost
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.67 REQUIRED COMPONENTS system filesystem program_options)

# GLib
include(FindPkgConfig)
pkg_check_modules(glib REQUIRED IMPORTED_TARGET glib-2.0)
pkg_check_modules(gmodule REQUIRED IMPORTED_TARGET gmodule-2.0)

# std::thread support
if (UNIX AND NOT APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()

# Copy config file to bin directory
configure_file(${SC_MACHINE_ROOT}/config/config.ini.in ${SC_BIN_PATH}/config.ini)

if (BUILD_TESTING)
    include (cmake/tests.cmake)
endif()

add_subdirectory(${SC_MACHINE_ROOT}/thirdparty)

add_subdirectory(${SC_MACHINE_ROOT}/sc-memory)
add_subdirectory(${SC_MACHINE_ROOT}/sc-kpm)
add_subdirectory(${SC_MACHINE_ROOT}/sc-server)
add_subdirectory(${SC_MACHINE_ROOT}/tools)

if (SC_BUILD_SCTP)
    add_subdirectory(${SC_MACHINE_ROOT}/sc-network)
endif()

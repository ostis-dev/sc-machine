#[[
- Try to find GLib2
Once done this will define

GLIB2_FOUND - system has GLib2
GLIB2_INCLUDE_DIRS - the GLib2 include directory
GLIB2_LIBRARIES - Link these to use GLib2

HAVE_GLIB_GREGEX_H  glib has gregex.h header and
                    supports g_regex_match_simple

Copyright (c) 2006 Andreas Schneider <mail@cynapses.org>
Copyright (c) 2006 Philippe Bernery <philippe.bernery@gmail.com>
Copyright (c) 2007 Daniel Gollub <gollub@b1-systems.de>
Copyright (c) 2007 Alban Browaeys <prahal@yahoo.com>
Copyright (c) 2008 Michael Bell <michael.bell@web.de>
Copyright (c) 2008-2009 Bjoern Ricks <bjoern.ricks@googlemail.com>

Redistribution and use is allowed according to the terms of the New
BSD license.
For details see the accompanying COPYING-CMAKE-SCRIPTS file.
]]

if(GLIB2_LIBRARIES AND GLIB2_INCLUDE_DIRS)
    # In cache already
    set(GLIB2_FOUND TRUE)
else(GLIB2_LIBRARIES AND GLIB2_INCLUDE_DIRS)
    include(FindPkgConfig)

    # Glib
    if(GLIB2_FIND_REQUIRED)
        set(_pkgconfig_REQUIRED "REQUIRED")
    else(GLIB2_FIND_REQUIRED)
        set(_pkgconfig_REQUIRED "")
    endif()

    if(GLIB2_MIN_VERSION)
        pkg_search_module(GLIB2 ${_pkgconfig_REQUIRED} glib-2.0>=${GLIB2_MIN_VERSION})
    else(GLIB2_MIN_VERSION)
        pkg_search_module(GLIB2 ${_pkgconfig_REQUIRED} glib-2.0)
    endif()
    if(PKG_CONFIG_FOUND)
        if(GLIB2_FOUND)
            set(GLIB2_CORE_FOUND TRUE)
        else(GLIB2_FOUND)
            set(GLIB2_CORE_FOUND FALSE)
        endif()
    endif()

    # Look for glib2 include dir and libraries w/o pkgconfig
    if(NOT GLIB2_FOUND AND NOT PKG_CONFIG_FOUND)
        find_path(
            _glibconfig_include_DIR
            NAMES glibconfig.h
            PATHS /opt/gnome/lib64
                  /opt/gnome/lib
                  /opt/lib/
                  /opt/local/lib
                  /sw/lib/
                  /usr/lib64
                  /usr/lib
                  /usr/local/include
                  /usr/local/lib
                  ${CMAKE_LIBRARY_PATH}
            PATH_SUFFIXES glib-2.0/include)

        find_path(
            _glib2_include_DIR
            NAMES glib.h
            PATHS /opt/gnome/include
                  /opt/local/include
                  /sw/include
                  /usr/include
                  /usr/local/include
            PATH_SUFFIXES glib-2.0)

        # MESSAGE(STATUS "Glib headers: ${_glib2_include_DIR}")

        find_library(
            _glib2_link_DIR
            NAMES glib-2.0 glib
            PATHS /opt/gnome/lib
                  /opt/local/lib
                  /sw/lib
                  /usr/lib
                  /usr/local/lib)
        if(_glib2_include_DIR AND _glib2_link_DIR)
            set(_glib2_FOUND TRUE)
        endif()

        if(_glib2_FOUND)
            set(GLIB2_INCLUDE_DIRS ${_glib2_include_DIR} ${_glibconfig_include_DIR})
            set(GLIB2_LIBRARIES ${_glib2_link_DIR})
            set(GLIB2_CORE_FOUND TRUE)
        else(_glib2_FOUND)
            set(GLIB2_CORE_FOUND FALSE)
        endif()

        # Handle dependencies libintl
        if(NOT LIBINTL_FOUND)
            find_path(
                LIBINTL_INCLUDE_DIR
                NAMES libintl.h
                PATHS /opt/gnome/include
                      /opt/local/include
                      /sw/include
                      /usr/include
                      /usr/local/include)

            find_library(
                LIBINTL_LIBRARY
                NAMES intl
                PATHS /opt/gnome/lib
                      /opt/local/lib
                      /sw/lib
                      /usr/local/lib
                      /usr/lib)

            if(LIBINTL_LIBRARY AND LIBINTL_INCLUDE_DIR)
                set(LIBINTL_FOUND TRUE)
            endif()
        endif()

        # libiconv
        if(NOT LIBICONV_FOUND)
            find_path(
                LIBICONV_INCLUDE_DIR
                NAMES iconv.h
                PATHS /opt/gnome/include
                      /opt/local/include
                      /opt/local/include
                      /sw/include
                      /sw/include
                      /usr/local/include
                      /usr/include
                PATH_SUFFIXES glib-2.0)

            find_library(
                LIBICONV_LIBRARY
                NAMES iconv
                PATHS /opt/gnome/lib
                      /opt/local/lib
                      /sw/lib
                      /usr/lib
                      /usr/local/lib)

            if(LIBICONV_LIBRARY AND LIBICONV_INCLUDE_DIR)
                set(LIBICONV_FOUND TRUE)
            endif()
        endif()

        if(LIBINTL_FOUND)
            set(GLIB2_LIBRARIES ${GLIB2_LIBRARIES} ${LIBINTL_LIBRARY})
            set(GLIB2_INCLUDE_DIRS ${GLIB2_INCLUDE_DIRS} ${LIBINTL_INCLUDE_DIR})
        endif()

        if(LIBICONV_FOUND)
            set(GLIB2_LIBRARIES ${GLIB2_LIBRARIES} ${LIBICONV_LIBRARY})
            set(GLIB2_INCLUDE_DIRS ${GLIB2_INCLUDE_DIRS} ${LIBICONV_INCLUDE_DIR})
        endif()
    endif()

    if(GLIB2_CORE_FOUND
       AND GLIB2_INCLUDE_DIRS
       AND GLIB2_LIBRARIES)
        set(GLIB2_FOUND TRUE)
    endif()

    if(GLIB2_FOUND)
        if(NOT GLIB2_FIND_QUIETLY)
            message(STATUS "Found GLib2: ${GLIB2_LIBRARIES} ${GLIB2_INCLUDE_DIRS}")
        endif()
    else(GLIB2_FOUND)
        if(GLIB2_FIND_REQUIRED)
            message(SEND_ERROR "Could not find GLib2")
        endif()
    endif()

    # Show the GLIB2_INCLUDE_DIRS and GLIB2_LIBRARIES variables only in the advanced view
    mark_as_advanced(GLIB2_INCLUDE_DIRS GLIB2_LIBRARIES)
    mark_as_advanced(LIBICONV_INCLUDE_DIR LIBICONV_LIBRARY)
    mark_as_advanced(LIBINTL_INCLUDE_DIR LIBINTL_LIBRARY)
endif()

if(WIN32)
    # Include libiconv for win32
    if(NOT LIBICONV_FOUND)
        find_path(LIBICONV_INCLUDE_DIR iconv.h PATH_SUFFIXES glib-2.0)

        find_library(LIBICONV_LIBRARY NAMES iconv)

        if(LIBICONV_LIBRARY AND LIBICONV_INCLUDE_DIR)
            set(LIBICONV_FOUND TRUE)
        endif()
    endif()
    if(LIBICONV_FOUND)
        set(GLIB2_LIBRARIES ${GLIB2_LIBRARIES} ${LIBICONV_LIBRARY})
        set(GLIB2_INCLUDE_DIRS ${GLIB2_INCLUDE_DIRS} ${LIBICONV_INCLUDE_DIR})
    endif()
endif()

if(GLIB2_FOUND)
    # Check if system has a newer version of glib which supports g_regex_match_simple
    include(CheckIncludeFiles)
    set(CMAKE_REQUIRED_INCLUDES ${GLIB2_INCLUDE_DIRS})
    check_include_files(glib/gregex.h HAVE_GLIB_GREGEX_H)
    check_include_files(glib/gchecksum.h HAVE_GLIB_GCHECKSUM_H)
    # Reset CMAKE_REQUIRED_INCLUDES
    set(CMAKE_REQUIRED_INCLUDES "")
endif()

# - Try to find GLib2Module
# Once done this will define
#
#  GLIB2_MODULE_FOUND - system has GLib2
#  GLIB2_MODULE_INCLUDE_DIRS - the GLib2 include directory
#  GLIB2_MODULE_LIBRARIES - Link these to use GLib2
#

if(GLIB2_MODULE_LIBRARIES AND GLIB2_MODULE_INCLUDE_DIRS)
    # In cache already
    set(GLIB2_MODULE_FOUND TRUE)
else(GLIB2_MODULE_LIBRARIES AND GLIB2_MODULE_INCLUDE_DIRS)
    include(FindPkgConfig)

    # Glib
    if(GLIB2_MODULE_FIND_REQUIRED)
        set(_pkgconfig_REQUIRED "REQUIRED")
    else(GLIB2_MODULE_FIND_REQUIRED)
        set(_pkgconfig_REQUIRED "")
    endif()

    # Look for glib2 include dir and libraries w/o pkgconfig
    if(NOT GLIB2_MODULE_FOUND AND NOT PKG_CONFIG_FOUND)
        find_path(_glib2_module_include_DIR
                  NAMES gmodule.h
                  PATHS /opt/gnome/include /opt/local/include /sw/include /usr/include /usr/local/include
                  PATH_SUFFIXES glib-2.0)

        # MESSAGE(STATUS "Glib headers: ${_glib2_include_DIR}")

        find_library(_glib2_module_link_DIR
                     NAMES gmodule-2.0 gmodule
                     PATHS /opt/gnome/lib /opt/local/lib /sw/lib /usr/lib /usr/local/lib)
        if(_glib2_module_include_DIR AND _glib2_module_link_DIR)
            set(_glib2_module_FOUND TRUE)
        endif()

        if(_glib2_module_FOUND)
            set(GLIB2_MODULE_INCLUDE_DIRS ${_glib2_module_include_DIR})
            set(GLIB2_MODULE_LIBRARIES ${_glib2_module_link_DIR})
            set(GLIB2_MODULE_FOUND TRUE)
        else(_glib2_module_FOUND)
            set(GLIB2_MODULE_FOUND FALSE)
        endif()
    endif()

    if(GLIB2_MODULE_FOUND)
        if(NOT GLIB2_MODULE_FIND_QUIETLY)
            message(STATUS "Found GLib2 module: ${GLIB2_MODULE_LIBRARIES} ${GLIB2_MODULE_INCLUDE_DIRS}")
        endif()
    else(GLIB2_MODULE_FOUND)
        if(GLIB2_MODEUL_FIND_REQUIRED)
            message(SEND_ERROR "Could not find GLib2 module")
        endif()
    endif()

    # Show the GLIB2_MODULE_INCLUDE_DIRS and GLIB2_MODULE_LIBRARIES variables only in the advanced view
    mark_as_advanced(GLIB2_MODULE_INCLUDE_DIRS GLIB2_MODULE_LIBRARIES)
endif()

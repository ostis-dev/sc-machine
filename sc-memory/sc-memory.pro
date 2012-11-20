TEMPLATE = lib
TARGET = $$qtLibraryTarget(sc_memory)

DESTDIR = ../bin

OBJECTS_DIR = obj
MOC_DIR = moc

HEADERS += \
    src/sc_memory.h \
    src/sc-store/sc_link_helpers.h \
    src/sc-store/sc_fs_storage.h \
    src/sc-store/sc_element.h \
    src/sc-store/sc_defines.h \
    src/sc-store/sc_config.h \
    src/sc-store/sc_types.h \
    src/sc-store/sc_stream_private.h \
    src/sc-store/sc_stream_file.h \
    src/sc-store/sc_stream.h \
    src/sc-store/sc_store.h \
    src/sc-store/sc_storage.h \
    src/sc-store/sc_segment.h \
    src/sc-store/sc_stream_memory.h \
    src/sc-store/sc_event.h \
    src/sc-store/sc_iterator3.h \
    src/sc-store/sc_iterator5.h \
    src/sc-store/sc_iterator.h \
    src/sc-store/sc_task_manager.h \
    src/sc-store/sc_thread.h

SOURCES += \
    src/sc_memory.c \
    src/sc-store/sc_link_helpers.c \
    src/sc-store/sc_fs_storage.c \
    src/sc-store/sc_element.c \
    src/sc-store/sc_stream_file.c \
    src/sc-store/sc_stream.c \
    src/sc-store/sc_storage.c \
    src/sc-store/sc_segment.c \
    src/sc-store/sc_stream_memory.c \
    src/sc-store/sc_event.c \
    src/sc-store/sc_iterator3.c \
    src/sc-store/sc_iterator5.c \
    src/sc-store/sc_task_manager.c \
    src/sc-store/sc_thread.c

win32 {
    GLIB_PATH = "c:/glib2.0"
    INCLUDEPATH += "$$GLIB_PATH/include/glib-2.0"
    INCLUDEPATH += "$$GLIB_PATH/lib/glib-2.0/include"

    POST_TARGETDEPS += "$$GLIB_PATH/lib/glib-2.0.lib"
    LIBS += "$$GLIB_PATH/lib/glib-2.0.lib"
}

unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += glib-2.0
}

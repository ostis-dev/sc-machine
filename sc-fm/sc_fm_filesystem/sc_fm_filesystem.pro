TEMPLATE = lib
TARGET = $$qtLibraryTarget(sc-fm-filesystem)

DESTDIR = ../../bin

OBJECTS_DIR = obj
MOC_DIR = moc

INCLUDEPATH += ../../sc-memory/src/sc-store

win32 {
    INCLUDEPATH += "../glib/include/glib-2.0"
    INCLUDEPATH += "../glib/lib/glib-2.0/include"

    POST_TARGETDEPS += ../glib/lib/glib-2.0.lib
    LIBS += ../glib/lib/glib-2.0.lib
}

unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += glib-2.0
    PKGCONFIG += gmodule-2.0
    LIBS += $$quote(-L$$BINDIR) -lsc_memory
}

SOURCES += \
    sc_fm_filesystem.c


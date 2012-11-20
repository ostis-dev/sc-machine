TEMPLATE = app
DESTDIR = ../../bin

INCLUDEPATH += ../../sc-memory/src/sc-store
unix {
    LIBS += $$quote(-L$$DESTDIR) -lsc_memory
    CONFIG += link_pkgconfig
    PKGCONFIG += glib-2.0
}

DEFINES += QT_COMPILATION

SOURCES += \
    test.cpp

win32 {
    CONFIG += qt console
    GLIB_PATH = "c:/glib2.0"
    INCLUDEPATH += "$$GLIB_PATH/include/glib-2.0"
    INCLUDEPATH += "$$GLIB_PATH/lib/glib-2.0/include"

    POST_TARGETDEPS += "$$GLIB_PATH/lib/glib-2.0.lib"
    LIBS += "$$GLIB_PATH/lib/glib-2.0.lib"

    LIBS += $$quote(-L$$DESTDIR) -lsc_memoryd
}

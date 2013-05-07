INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

qtlockedfile-uselib:!qtlockedfile-buildlib {
	LIBS += -L$$LIBDIR -l$$qtLibraryTarget(qtlockedfile)
} else {
    HEADERS += $$PWD/qtlockedfile.h
    SOURCES += $$PWD/qtlockedfile.cpp
    unix:SOURCES += $$PWD/qtlockedfile_unix.cpp
    win32:SOURCES += $$PWD/qtlockedfile_win.cpp
}

win32 {
    contains(TEMPLATE, lib):contains(CONFIG, shared):DEFINES += QT_QTLOCKEDFILE_EXPORT
    else:qtlockedfile-uselib:DEFINES += QT_QTLOCKEDFILE_IMPORT
}

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

qtlockedfile-uselib:!qtlockedfile-buildlib {
	LIBS += -L$$LIBDIR -l$$qtLibraryTarget(qtlockedfile)
} else {
	HEADERS += \
		qtlockedfile.h

	SOURCES += \
		qtlockedfile.cpp

	unix:SOURCES += \
		qtlockedfile_unix.cpp

	win32:SOURCES += \
		qtlockedfile_win.cpp
}

win32 {
    contains(TEMPLATE, lib):contains(CONFIG, shared):DEFINES += QT_QTLOCKEDFILE_EXPORT
    else:qtlockedfile-uselib:DEFINES += QT_QTLOCKEDFILE_IMPORT
}

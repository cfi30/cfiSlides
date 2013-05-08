QT += core gui
TARGET = cfiSlides
TEMPLATE = app
DESTDIR = ../build/
LIBDIR = $$DESTDIR

VERSION = 0.3
DEFINES += CFISLIDES_VERSION=\\\"$$VERSION\\\"

SOURCES += \
	main.cpp

TRANSLATIONS = \
	strings_fr.ts \
	strings_en.ts \
	qt_fr.ts

RESOURCES += \
	cfiSlides.qrc

EXTRA_BINFILES += $$PWD/cfiSlides.sh
unix:QMAKE_POST_LINK += $$QMAKE_COPY $$PWD/cfiSlides.sh $$DESTDIR

CONFIG += cfislides-uselib
include(../shared/shared.pri)

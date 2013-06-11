QT += core gui phonon
TARGET = cfiSlides
TEMPLATE = app
DESTDIR = ../build/
LIBDIR = $$DESTDIR

VERSION = 0.4
DEFINES += CFISLIDES_VERSION=\\\"$$VERSION\\\"

HEADERS += \
	mainwindow.h \
	graphicsview.h \
	imageelement.h \
	rectelement.h \
	textelement.h \
	movieelement.h \
	ellipseelement.h \
	viewwidget.h \
	audioelement.h \
	lineelement.h \
	plugindialog.h \
	plugin.h \
	shared.h \
	geometrydialog.h \

SOURCES += \
	main.cpp \
	mainwindow.cpp \
	graphicsview.cpp \
	imageelement.cpp \
	rectelement.cpp \
	textelement.cpp \
	movieelement.cpp \
	ellipseelement.cpp \
	viewwidget.cpp \
	audioelement.cpp \
	lineelement.cpp \
	plugindialog.cpp \
	geometrydialog.cpp \

FORMS += \
	mainwindow.ui \
	viewwidget.ui \
	plugindialog.ui \
	geometrydialog.ui \

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

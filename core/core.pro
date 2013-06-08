QT += core gui phonon
TARGET = cfiSlides
TEMPLATE = app
DESTDIR = ../build/
LIBDIR = $$DESTDIR

VERSION = 0.3
DEFINES += CFISLIDES_VERSION=\\\"$$VERSION\\\"

HEADERS += \
	mainwindow.h \
	graphicsview.h \
	imageelement.h \
	graphicsitem.h \
	graphicspixmapitem.h \
	graphicsrectitem.h \
	rectelement.h \
	graphicstextitem.h \
	textelement.h \
	movieelement.h \
	graphicsellipseitem.h \
	ellipseelement.h \
	viewwidget.h \
	audioelement.h \
	graphicslineitem.h \
	lineelement.h \
	plugindialog.h \
	plugin.h \
	shared.h \
	graphicsmoviepreviewitem.h \
	geometrydialog.h \

SOURCES += \
	main.cpp \
	mainwindow.cpp \
	graphicsview.cpp \
	imageelement.cpp \
	graphicspixmapitem.cpp \
	graphicsrectitem.cpp \
	rectelement.cpp \
	graphicstextitem.cpp \
	textelement.cpp \
	movieelement.cpp \
	graphicsellipseitem.cpp \
	ellipseelement.cpp \
	viewwidget.cpp \
	audioelement.cpp \
	graphicslineitem.cpp \
	lineelement.cpp \
	plugindialog.cpp \
	graphicsmoviepreviewitem.cpp \
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

QT += core gui widgets \
	printsupport multimediawidgets

TARGET = cfislides
TEMPLATE = app
DESTDIR = ../build/
LIBDIR = $$DESTDIR

VERSION = 0.3
DEFINES += CFISLIDES_VERSION=\\\"$$VERSION\\\"

HEADERS += \
	mainwindow.h \
	graphicsview.h \
	imageelement.h \
	rectelement.h \
	textelement.h \
	videoelement.h \
	ellipseelement.h \
	viewwidget.h \
	audioelement.h \
	lineelement.h \
	plugindialog.h \
	../shared/plugin.h \
	geometrydialog.h \

SOURCES += \
	main.cpp \
	mainwindow.cpp \
	graphicsview.cpp \
	imageelement.cpp \
	rectelement.cpp \
	textelement.cpp \
	videoelement.cpp \
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
	core.qrc

EXTRA_BINFILES += $$PWD/cfislides.sh
unix:QMAKE_POST_LINK += $$QMAKE_COPY $$PWD/cfislides.sh $$DESTDIR

CONFIG += cfislides-uselib
include(../shared/shared.pri)

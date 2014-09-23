QT += core gui widgets \
	printsupport multimediawidgets

TEMPLATE = app
CONFIG += c++11
TARGET = cfislides
DESTDIR = ../build/
LIBDIR = $$DESTDIR

VERSION = 0.5
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
	resizedialog.h \
	../shared/plugin.h \

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
	resizedialog.cpp \

FORMS += \
	mainwindow.ui \
	viewwidget.ui \
	plugindialog.ui \
	resizedialog.ui \

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

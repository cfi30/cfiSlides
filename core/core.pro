QT += core gui phonon
TARGET = cfiSlides
TEMPLATE = app
DESTDIR = ../build/
LIBDIR = $$DESTDIR

VERSION = 0.3
DEFINES += CFISLIDES_VERSION=\\\"$$VERSION\\\"

HEADERS += \
	mainwindow.h \
	slideshow.h \
	slide.h \
	graphicsview.h \
	baseelement.h \
	slideelement.h \
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
	slideshowelement.h \
	geometrydialog.h \
	property.h \
	propertymanager.h \
	propertyeditor.h \
	propertyeditordelegate.h \

SOURCES += \
	main.cpp \
	mainwindow.cpp \
	slideshow.cpp \
	slide.cpp \
	graphicsview.cpp \
	baseelement.cpp \
	slideelement.cpp \
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
	slideshowelement.cpp \
	geometrydialog.cpp \
	property.cpp \
	propertymanager.cpp \
	propertyeditor.cpp \
	propertyeditordelegate.cpp \

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

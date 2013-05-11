INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

cfislides-uselib:!cfislides-buildlib {
	LIBS += -L$$LIBDIR -l$$qtLibraryTarget(cfislides)
} else {
	HEADERS += \
		mainwindow.h \
		slideshow.h \
		slide.h \
		graphicsview.h \
		configuration.h \
		baseelement.h \
		slideelement.h \
		imageelement.h \
		graphicsitem.h \
		graphicspixmapitem.h \
		graphicsrectitem.h \
		rectelement.h \
		graphicstextitem.h \
		textelement.h \
		textinputdialog.h \
		texteditfactory.h \
		textedit.h \
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

	SOURCES += \
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
		textinputdialog.cpp \
		texteditfactory.cpp \
		textedit.cpp \
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

	FORMS += \
		mainwindow.ui \
		textinputdialog.ui \
		viewwidget.ui \
		plugindialog.ui \
		geometrydialog.ui \
}

win32 {
    contains(TEMPLATE, lib):contains(CONFIG, shared):DEFINES += CFISLIDES_EXPORT
	else:cfislides-uselib:DEFINES += CFISLIDES_IMPORT
}

CONFIG += qtpropertybrowser-uselib
include(../qtpropertybrowser/qtpropertybrowser.pri)
include(../qtpropertybrowser/extension/extension.pri)

CONFIG += qtlockedfile-uselib
include(../qtlockedfile/qtlockedfile.pri)

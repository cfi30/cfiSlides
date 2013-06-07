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
		textinputdialog.ui \
		viewwidget.ui \
		plugindialog.ui \
		geometrydialog.ui \
}

win32 {
    contains(TEMPLATE, lib):contains(CONFIG, shared):DEFINES += CFISLIDES_EXPORT
	else:cfislides-uselib:DEFINES += CFISLIDES_IMPORT
}

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

cfislides-uselib:!cfislides-buildlib {
	LIBS += -L$$LIBDIR -l$$qtLibraryTarget(cfislides)
} else {
	HEADERS += \
		slideshow.h \
		slide.h \
		configuration.h \
		baseelement.h \
		slideelement.h \
		slideelementtype.h \
		graphicsitem.h \
		textinputdialog.h \
		plugin.h \
		shared.h \
		slideshowelement.h \
		property.h \
		propertymanager.h \
		propertyeditor.h \
		propertyeditordelegate.h \
		icon_t.h \

	SOURCES += \
		slideshow.cpp \
		slide.cpp \
		baseelement.cpp \
		slideelement.cpp \
		slideelementtype.cpp \
		textinputdialog.cpp \
		slideshowelement.cpp \
		property.cpp \
		propertymanager.cpp \
		propertyeditor.cpp \
		propertyeditordelegate.cpp \

	FORMS += \
		textinputdialog.ui \
}

win32 {
    contains(TEMPLATE, lib):contains(CONFIG, shared):DEFINES += CFISLIDES_EXPORT
	else:cfislides-uselib:DEFINES += CFISLIDES_IMPORT
}

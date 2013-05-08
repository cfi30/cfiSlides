INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

qtpropertybrowser-uselib:!qtpropertybrowser-buildlib {
	LIBS += -L$$LIBDIR -l$$qtLibraryTarget(qtpropertybrowser)
} else {
	HEADERS += \
		qtpropertybrowser.h \
		qtpropertymanager.h \
		qteditorfactory.h \
		qtvariantproperty.h \
		qttreepropertybrowser.h \
		qtbuttonpropertybrowser.h \
		qtgroupboxpropertybrowser.h \
		qtpropertybrowserutils_p.h

	SOURCES += \
		qtpropertybrowser.cpp \
		qtpropertymanager.cpp \
		qteditorfactory.cpp \
		qtvariantproperty.cpp \
		qttreepropertybrowser.cpp \
		qtbuttonpropertybrowser.cpp \
		qtgroupboxpropertybrowser.cpp \
		qtpropertybrowserutils.cpp

	RESOURCES += \
		qtpropertybrowser.qrc
}

win32 {
    contains(TEMPLATE, lib):contains(CONFIG, shared):DEFINES += QT_QTPROPERTYBROWSER_EXPORT
    else:qtpropertybrowser-uselib:DEFINES += QT_QTPROPERTYBROWSER_IMPORT
}

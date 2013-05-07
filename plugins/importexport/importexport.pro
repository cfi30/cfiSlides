include(../plugins.pri)

QT += core gui
TARGET = import_export
VERSION = 1.0

HEADERS += \
	importexport.h \
	exportdialog.h \
    importdialog.h

SOURCES += \
	importexport.cpp \
	exportdialog.cpp \
    importdialog.cpp

FORMS += \
	exportdialog.ui \
    importdialog.ui

RESOURCES += \
	importexport.qrc

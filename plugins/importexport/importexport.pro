include(../plugins.pri)

QT += core gui widgets
TARGET = importexport

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

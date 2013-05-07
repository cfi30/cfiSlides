include(../plugins.pri)

QT += core gui
TARGET = hello_world
VERSION = 1.0

HEADERS += \
	helloworld.h \
    helloelement.h \
    hello2ditem.h

SOURCES += \
	helloworld.cpp \
    helloelement.cpp \
    hello2ditem.cpp

RESOURCES += \
	helloworld.qrc

include(../plugins.pri)

QT += core gui
TARGET = hello_world
VERSION = 1.0

HEADERS += \
	helloworld.h \
	helloelement.h \

SOURCES += \
	helloworld.cpp \
	helloelement.cpp \

RESOURCES += \
	helloworld.qrc

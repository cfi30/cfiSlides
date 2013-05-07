TEMPLATE = lib
CONFIG += shared plugin qtpropertybrowser-buildlib
TARGET = qtpropertybrowser
DESTDIR = ../build/
LIBDIR = $$DESTDIR

include(qtpropertybrowser.pri)
include(extension/extension.pri)

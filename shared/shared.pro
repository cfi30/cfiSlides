QT  += core gui widgets
TEMPLATE = lib
CONFIG += shared plugin c++11 cfislides-buildlib
TARGET = cfislides
DESTDIR = ../build/
LIBDIR = $$DESTDIR

include(shared.pri)

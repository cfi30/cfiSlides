TEMPLATE = lib
CONFIG += plugin c++11
DESTDIR = ../../build/plugins/
LIBDIR = ../../build/

INCLUDEPATH += \
	../../shared/

HEADERS += \
	../../shared/plugin.h

CONFIG += cfislides-uselib
include(../shared/shared.pri)

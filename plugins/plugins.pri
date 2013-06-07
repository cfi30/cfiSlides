TEMPLATE = lib
CONFIG += plugin
DESTDIR = ../../build/plugins/
LIBDIR = ../../build/

INCLUDEPATH += \
	../../shared/

HEADERS += \
	../../shared/plugin.h

CONFIG += cfislides-uselib
include(../shared/shared.pri)

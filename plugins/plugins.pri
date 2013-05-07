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

CONFIG += qtpropertybrowser-uselib
include(../qtpropertybrowser/qtpropertybrowser.pri)
include(../qtpropertybrowser/extension/extension.pri)

CONFIG += qtlockedfile-uselib
include(../qtlockedfile/qtlockedfile.pri)

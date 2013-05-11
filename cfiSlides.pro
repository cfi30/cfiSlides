TEMPLATE = subdirs
CONFIG += ordered build-plugins
SUBDIRS += \
	qtpropertybrowser \
	shared \
	core

build-plugins {
	SUBDIRS += \
		plugins/helloworld \
		plugins/importexport
}

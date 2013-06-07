TEMPLATE = subdirs
CONFIG += ordered build-plugins
SUBDIRS += \
	shared \
	core

build-plugins {
	SUBDIRS += \
		plugins/helloworld \
		plugins/importexport
}

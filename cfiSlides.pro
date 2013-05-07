TEMPLATE = subdirs
CONFIG += ordered build-plugins
SUBDIRS += \
	qtpropertybrowser \
	qtlockedfile \
	shared \
	core

build-plugins {
	SUBDIRS += \
		plugins/helloworld \
		plugins/importexport
}

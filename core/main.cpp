/**
 * Copyright (C) 2013  Christian Fillion
 * This file is part of cfiSlides.
 *
 * cfiSlides is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * cfiSlides is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with cfiSlides.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QApplication>
#include <QTextCodec>
#include <QLocale>
#include <QTranslator>

#include "mainwindow.h"
#include "configuration.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	app.setApplicationName("cfiSlides");
	app.setApplicationVersion(CFISLIDES_VERSION);
	app.setOrganizationName("cfi30");

	const QString locale = QLocale::system().name().section('_', 0, 0);

	QTranslator qt_translator;
	qt_translator.load("qt_" + locale, "lang");
	app.installTranslator(&qt_translator);

	QTranslator app_translator;
	qt_translator.load("lang_" + locale, "lang");
	app.installTranslator(&app_translator);

	if(!QIcon::hasThemeIcon(TEST_ICON))
		QIcon::setThemeName(FALLBACK_THEME);

	QStringList arguments = app.arguments();

	bool disablePlugins = arguments.contains("--noplugins");
	arguments.removeAll("--noplugins");

	MainWindow window(arguments.size() > 1 ? arguments[1] : QString(), disablePlugins);
	window.show();

	return app.exec();
}

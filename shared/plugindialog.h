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

#ifndef PLUGINDIALOG_H
#define PLUGINDIALOG_H

#include <QDialog>
#include <QTimer>
#include <QDir>
#include <QDebug>
#include <QDesktopServices>
#include <QUrl>
#include <QSettings>
#include <QPluginLoader>

#include "plugin.h"
#include "configuration.h"
#include "shared.h"

namespace Ui {
	class PluginDialog;
}

class CFISLIDES_DLLSPEC PluginDialog : public QDialog
{
	Q_OBJECT
	
public:
	explicit PluginDialog(QWidget *parent = 0);
	~PluginDialog();
	QStringList getEnabledPlugins();
	
private:
	Ui::PluginDialog *ui;
	QStringList availablePlugins;
	QStringList enabledPlugins;

private slots:
	void loadPlugins();
	void on_openDirectory_clicked();
	void on_availablePluginsList_currentRowChanged(int currentRow);
	void on_enabledPluginsList_currentRowChanged(int currentRow);
	void on_enableButton_clicked();
	void on_disableButton_clicked();
};

#endif // PLUGINDIALOG_H

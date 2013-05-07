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

#include "plugindialog.h"
#include "ui_plugindialog.h"

PluginDialog::PluginDialog(QWidget *parent) : QDialog(parent), ui(new Ui::PluginDialog)
{
	ui->setupUi(this);
	ui->openDirectory->setToolTip(PLUGINS_PATH);
	this->setFixedSize(this->minimumSize());

	QTimer::singleShot(REFRESH_INTERVAL, this, SLOT(loadPlugins()));
}

PluginDialog::~PluginDialog()
{
	delete ui;
}

QStringList PluginDialog::getEnabledPlugins()
{
	return enabledPlugins;
}

void PluginDialog::loadPlugins()
{
	QDir dir(PLUGINS_PATH);
	QStringList allPlugins = dir.entryList(QDir::Files, QDir::Time | QDir::Reversed);
	QStringList enabledOnes = QSettings().value("plugins").toStringList();

	foreach(QString fileName, allPlugins)
	{
		qDebug() << "fetching plugin name for" << fileName;
		QString pluginName;
		QPluginLoader loader(dir.absoluteFilePath(fileName));
		Plugin* plugin = qobject_cast<Plugin *>(loader.instance());
		if(plugin)
			pluginName = plugin->name();
		else
			qDebug() << fileName << loader.errorString();
		loader.unload();

		if(pluginName.isEmpty())
			continue;

		if(enabledOnes.contains(fileName))
		{
			enabledPlugins.append(fileName);
			ui->enabledPluginsList->addItem(pluginName);
		}
		else
		{
			availablePlugins.append(fileName);
			ui->availablePluginsList->addItem(pluginName);
		}
	}
}

void PluginDialog::on_openDirectory_clicked()
{
	QDesktopServices::openUrl(QUrl::fromLocalFile(PLUGINS_PATH));
}

void PluginDialog::on_availablePluginsList_currentRowChanged(int currentRow)
{
	ui->enableButton->setEnabled(currentRow > -1);
}

void PluginDialog::on_enabledPluginsList_currentRowChanged(int currentRow)
{
	ui->disableButton->setEnabled(currentRow > -1);
}

void PluginDialog::on_enableButton_clicked()
{
	int currentRow = ui->availablePluginsList->currentRow();
	QListWidgetItem *item = ui->availablePluginsList->item(currentRow);

	enabledPlugins.append(availablePlugins[currentRow]);
	ui->enabledPluginsList->addItem(item->text());

	availablePlugins.removeAt(currentRow);
	ui->availablePluginsList->setCurrentRow(-1);
	delete item;

	ui->enabledPluginsList->setCurrentRow(ui->enabledPluginsList->count() - 1);
	ui->enabledPluginsList->scrollToBottom();
}

void PluginDialog::on_disableButton_clicked()
{
	int currentRow = ui->enabledPluginsList->currentRow();
	QListWidgetItem *item = ui->enabledPluginsList->item(currentRow);

	availablePlugins.append(enabledPlugins[currentRow]);
	ui->availablePluginsList->addItem(item->text());

	enabledPlugins.removeAt(currentRow);
	ui->enabledPluginsList->setCurrentRow(-1);
	delete item;

	ui->availablePluginsList->setCurrentRow(ui->availablePluginsList->count() - 1);
	ui->availablePluginsList->scrollToBottom();
}

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

#include <QTimer>
#include <QDir>
#include <QDesktopServices>
#include <QUrl>
#include <QSettings>
#include <QPluginLoader>
#include <QJsonObject>

#include "plugindialog.h"
#include "ui_plugindialog.h"
#include "plugin.h"
#include "configuration.h"

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
	const QDir dir(PLUGINS_PATH);
	const QStringList allPlugins = dir.entryList(QDir::Files, QDir::Time | QDir::Reversed);
	const QStringList enabledOnes = QSettings().value(QStringLiteral("plugins")).toStringList();

	foreach(const QString fileName, allPlugins)
	{
		QPluginLoader loader(dir.absoluteFilePath(fileName));
		const QJsonObject metaData = loader.metaData().value(QStringLiteral("MetaData")).toObject();
		const QString name = metaData.value(QStringLiteral("name")).toString();
		const QString about = metaData.value(QStringLiteral("about")).toString();
		const QString version = metaData.value(QStringLiteral("version")).toString();

		if(name.isEmpty())
			continue;

		QListWidgetItem *item = new QListWidgetItem(tr("%1 (%3)").arg(name, version));
		item->setToolTip(about);

		if(enabledOnes.contains(fileName))
		{
			enabledPlugins << fileName;
			ui->enabledPluginsList->addItem(item);
		}
		else
		{
			availablePlugins << fileName;
			ui->availablePluginsList->addItem(item);
		}
	}

	ui->buttonBox->setEnabled(true);
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
	const int currentRow = ui->availablePluginsList->currentRow();
	QListWidgetItem *item = ui->availablePluginsList->item(currentRow);

	enabledPlugins << availablePlugins[currentRow];
	ui->enabledPluginsList->addItem(item->text());

	availablePlugins.removeAt(currentRow);
	ui->availablePluginsList->setCurrentRow(-1);
	delete item;

	ui->enabledPluginsList->setCurrentRow(ui->enabledPluginsList->count() - 1);
	ui->enabledPluginsList->scrollToBottom();
}

void PluginDialog::on_disableButton_clicked()
{
	const int currentRow = ui->enabledPluginsList->currentRow();
	QListWidgetItem *item = ui->enabledPluginsList->item(currentRow);

	availablePlugins << enabledPlugins[currentRow];
	ui->availablePluginsList->addItem(item->text());

	enabledPlugins.removeAt(currentRow);
	ui->enabledPluginsList->setCurrentRow(-1);
	delete item;

	ui->availablePluginsList->setCurrentRow(ui->availablePluginsList->count() - 1);
	ui->availablePluginsList->scrollToBottom();
}

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

#ifndef IMPORTDIALOG_H
#define IMPORTDIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QTreeWidgetItem>
#include <QSettings>
#include <QFileInfo>

#include "mainwindow.h"
#include "slide.h"
#include "configuration.h"

namespace Ui {
	class ImportDialog;
}

class ImportDialog : public QDialog
{
	Q_OBJECT
	
public:
	explicit ImportDialog(int slideCount, QWidget *parent = 0);
	~ImportDialog();
	QList<Slide *> slides();
	
private:
	Ui::ImportDialog *ui;

	enum ElementType
	{
		NullType,
		ImageType,
		MovieType,
		AudioType
	};

	int slideCount;
	int previousFilter;
	int previousSort;
	bool modified;

	bool updateList(const QString directory);
	const QStringList parseFilter(QString filter);
	ElementType typeOf(const QString file);
	SlideElement *createElementFor(const QString file);
	QIcon getIconFor(const QString file);
	void garbageCollector(bool destroyEnabledElements = true);

private slots:
	void enableOkButton();
	void elementModified();
	void on_directoryButton_clicked();
	void on_filterComboBox_currentIndexChanged(int index);
	void on_sortComboBox_currentIndexChanged(int index);
	void on_treeWidget_itemChanged(QTreeWidgetItem *item, int);
	void on_propertiesButton_toggled(bool checked);
	void on_treeWidget_itemSelectionChanged();
};

#endif // IMPORTDIALOG_H
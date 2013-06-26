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

#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H

#include <QDialog>

namespace Ui
{
	class ExportDialog;
}

class ExportDialog : public QDialog
{
	Q_OBJECT
	
public:
	enum SelectionMode
	{
		AllSlides,
		CustomRange,
		CurrentSlide
	};

	explicit ExportDialog(QWidget *parent = 0);
	~ExportDialog();
	void setSlideCount(int max);
	QString format() const;
	int quality() const;
	QString directory() const;
	QString fileTemplate() const;
	SelectionMode selectionMode() const;
	int from() const;
	int to() const;
	
private slots:
	void validate();
	void enableOkButton();
	void on_toolButton_clicked();

private:
	Ui::ExportDialog *ui;
};

#endif // EXPORTDIALOG_H

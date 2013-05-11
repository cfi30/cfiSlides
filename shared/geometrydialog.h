/**
 * Copyright (C) 2013  Christian Fillion
 * This file is part of Foobar.
 *
 * Foobar is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Foobar is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GEOMETRYDIALOG_H
#define GEOMETRYDIALOG_H

#include <QDialog>
#include <QDesktopWidget>

namespace Ui {
	class GeometryDialog;
}

class GeometryDialog : public QDialog
{
	Q_OBJECT
	
public:
	explicit GeometryDialog(QRect rect, QWidget *parent = 0);
	~GeometryDialog();
	QRect getRect() const;

private slots:
	void widthChanged(int);
	void heightChanged(int);

private:
	Ui::GeometryDialog *ui;
	int widthCache;
	int heightCache;
};

#endif // GEOMETRYDIALOG_H
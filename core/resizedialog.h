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

#ifndef RESIZEDIALOG_H
#define RESIZEDIALOG_H

#include <QDialog>

class QScreen;
class Slideshow;

namespace Ui {
	class ResizeDialog;
}

class ResizeDialog : public QDialog
{
	Q_OBJECT
	
public:
	explicit ResizeDialog(const QSize &currentSize, QWidget *parent = 0);
	~ResizeDialog();
	QSize getSize() const;

private slots:
	void selectedScreenChanged(int);
	void lockRatioToggled(bool);
	void widthChanged(int);
	void heightChanged(int);

private:
	void addScreen(const QScreen *);
	Ui::ResizeDialog *ui;
	QList<QSize> screenSizes;
	double ratio;
};

#endif // RESIZEDIALOG_H

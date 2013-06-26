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

#ifndef TEXTINPUTDIALOG_H
#define TEXTINPUTDIALOG_H

#include <QDialog>
#include "shared.h"

namespace Ui
{
	class TextInputDialog;
}

class CFISLIDES_DLLSPEC TextInputDialog : public QDialog
{
	Q_OBJECT
	
public:
	explicit TextInputDialog(QWidget *parent = 0);
	~TextInputDialog();
	void setText(const QString &text);
	QString text() const;
	
private slots:
	void on_plainTextEdit_textChanged();

private:
	Ui::TextInputDialog *ui;
};

#endif // TEXTINPUTDIALOG_H

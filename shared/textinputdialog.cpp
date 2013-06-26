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

#include <QPushButton>

#include "textinputdialog.h"
#include "ui_textinputdialog.h"

TextInputDialog::TextInputDialog(QWidget *parent) : QDialog(parent), ui(new Ui::TextInputDialog)
{
	ui->setupUi(this);
	on_plainTextEdit_textChanged();
}

TextInputDialog::~TextInputDialog()
{
	delete ui;
}

void TextInputDialog::setText(const QString &text)
{
	ui->plainTextEdit->setPlainText(text);
	on_plainTextEdit_textChanged();
}

QString TextInputDialog::text() const
{
	return ui->plainTextEdit->toPlainText().trimmed();
}

void TextInputDialog::on_plainTextEdit_textChanged()
{
	ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!text().isEmpty());
}

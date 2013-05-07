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

#include "textedit.h"

TextEdit::TextEdit(QWidget *parent) : QWidget(parent)
{
	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->setMargin(0);
	layout->setSpacing(0);

	theLabel = new QLabel(this);
	theLabel->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred));
	theLabel->setContentsMargins(3, 2, 0, 2);
	setFocusProxy(theLabel);
	setFocusPolicy(Qt::StrongFocus);

	QToolButton *button = new QToolButton(this);
	button->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred));
	button->setText("...");
	connect(button, SIGNAL(clicked()), this, SLOT(buttonClicked()));

	layout->addWidget(theLabel);
	layout->addWidget(button);

	setAttribute(Qt::WA_InputMethodEnabled);
}

void TextEdit::setText(const QString &text)
{
	theLabel->setText(text);
}

void TextEdit::buttonClicked()
{
	TextInputDialog *dialog = new TextInputDialog(this);
	dialog->setWindowTitle(tr("Modifier le texte"));
	dialog->setText(theLabel->text());
	if (dialog->exec() == QDialog::Rejected)
		return;

	theLabel->setText(dialog->text());
	emit textChanged(dialog->text());
}


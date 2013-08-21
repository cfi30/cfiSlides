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

#include <QScreen>

#include "resizedialog.h"
#include "ui_resizedialog.h"
#include "slideshow.h"

ResizeDialog::ResizeDialog(const QSize &currentSize, QWidget *parent) : QDialog(parent), ui(new Ui::ResizeDialog)
{
	ui->setupUi(this);
	connect(qApp, &QApplication::screenAdded, this, &ResizeDialog::addScreen);

	foreach(const QScreen *screen, qApp->screens())
		addScreen(screen);

	const int screenIndex = screenSizes.indexOf(currentSize);
	if(screenIndex > -1)
	{
		ui->automaticSize->setChecked(true);
		ui->screenList->setCurrentIndex(screenIndex);
	}
	else
	{
		ui->customSize->setChecked(true);
		ui->width->setFocus();
	}

	ui->width->blockSignals(true);
	ui->width->setValue(currentSize.width());
	ui->width->blockSignals(false);

	ui->height->blockSignals(true);
	ui->height->setValue(currentSize.height());
	ui->height->blockSignals(false);
}

ResizeDialog::~ResizeDialog()
{
	delete ui;
}

void ResizeDialog::addScreen(const QScreen *screen)
{
	screenSizes << screen->size();
	ui->screenList->addItem(
		QString("%1 (%2 x %3)")
		.arg(screen->name())
		.arg(screen->size().width())
		.arg(screen->size().height())
	);
}

QSize ResizeDialog::getSize() const
{
	if(ui->automaticSize->isChecked())
		return screenSizes[ui->screenList->currentIndex()];
	else
		return QSize(ui->width->value(), ui->height->value());
}

void ResizeDialog::selectedScreenChanged(int)
{
	ui->automaticSize->setChecked(true);
}

void ResizeDialog::lockRatioToggled(bool checked)
{
	ui->width->setFocus();
	if(checked) ratio = (double)ui->width->value() / ui->height->value();
}

void ResizeDialog::widthChanged(int value)
{
	ui->customSize->setChecked(true);
	if(!ui->lockRatio->isChecked())
		return;

	ui->height->blockSignals(true);
	ui->height->setValue((double)value / ratio);
	ui->height->blockSignals(false);
}

void ResizeDialog::heightChanged(int value)
{
	ui->customSize->setChecked(true);
	if(!ui->lockRatio->isChecked())
		return;

	ui->width->blockSignals(true);
	ui->width->setValue((double)value * ratio);
	ui->width->blockSignals(false);
}

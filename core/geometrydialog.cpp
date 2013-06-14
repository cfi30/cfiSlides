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

#include "geometrydialog.h"
#include "ui_geometrydialog.h"

GeometryDialog::GeometryDialog(QRect rect, QWidget *parent) : QDialog(parent), ui(new Ui::GeometryDialog)
{
	ui->setupUi(this);
	if(rect.isNull())
	{
		ui->automatic->setChecked(true);
		rect = QDesktopWidget().screenGeometry();
	}
	else
		ui->custom->setChecked(true);

	ui->width->blockSignals(true);
	ui->width->setValue(rect.width());
	ui->width->blockSignals(false);

	ui->height->blockSignals(true);
	ui->height->setValue(rect.height());
	ui->height->blockSignals(false);
}

GeometryDialog::~GeometryDialog()
{
	delete ui;
}

QRect GeometryDialog::getRect() const
{
	QRect rect;
	if(ui->custom->isChecked())
		rect = QRect(QPoint(), QSize(ui->width->value(), ui->height->value()));
	return rect;
}

void GeometryDialog::lockRatioToggled(bool checked)
{
	ui->width->setFocus();
	if(checked) ratio = (double)ui->width->value() / ui->height->value();
}

void GeometryDialog::widthChanged(int value)
{
	ui->custom->setChecked(true);
	if(!ui->lockRatio->isChecked())
		return;

	ui->height->blockSignals(true);
	ui->height->setValue((double)value / ratio);
	ui->height->blockSignals(false);
}

void GeometryDialog::heightChanged(int value)
{
	ui->custom->setChecked(true);
	if(!ui->lockRatio->isChecked())
		return;

	ui->width->blockSignals(true);
	ui->width->setValue((double)value * ratio);
	ui->width->blockSignals(false);
}

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
#include <QFileDialog>
#include <QImageWriter>
#include <QSettings>
#include <QMessageBox>

#include "exportdialog.h"
#include "ui_exportdialog.h"

ExportDialog::ExportDialog(QWidget *parent) :  QDialog(parent), ui(new Ui::ExportDialog)
{
	ui->setupUi(this);
	this->setFixedSize(this->minimumSize());

	int formatIndex = 0;
	foreach(QString format, QImageWriter::supportedImageFormats())
	{
		ui->formatComboBox->addItem(format);
		if(format.toLower() == QSettings().value(QStringLiteral("exportDialog/format"), "png").toString())
			ui->formatComboBox->setCurrentIndex(formatIndex);
		formatIndex++;
	}
	ui->qualitySlider->setValue(QSettings().value(QStringLiteral("exportDialog/quality"), 100).toInt());
	ui->templateLineEdit->setText(QSettings().value(QStringLiteral("exportDialog/template"), tr("Diapositive %i - %n.%f")).toString());
	ui->templateLineEdit->setWhatsThis(ui->templateLineEdit->toolTip());
	ui->templateLineEdit->setValidator(new QRegExpValidator(QRegExp(QStringLiteral("^[\\w\\s%\\._-]+$")), this));
	ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}

ExportDialog::~ExportDialog()
{
	delete ui;
}

void ExportDialog::setSlideCount(int max)
{
	ui->fromSpinBox->setMaximum(max);
	ui->toSpinBox->setMaximum(max);
	ui->toSpinBox->setValue(max);
}

QString ExportDialog::format() const
{
	return ui->formatComboBox->currentText();
}

int ExportDialog::quality() const
{
	return ui->qualitySlider->value();
}

QString ExportDialog::directory() const
{
	return ui->directoryLineEdit->text();
}

QString ExportDialog::fileTemplate() const
{
	return ui->templateLineEdit->text();
}

ExportDialog::SelectionMode ExportDialog::selectionMode() const
{
	if(ui->rangeRadioButton->isChecked())
		return CustomRange;
	else if(ui->currentSlideRadioButton->isChecked())
		return CurrentSlide;

	return AllSlides;
}

int ExportDialog::from() const
{
	switch(selectionMode())
	{
		case CustomRange:
			return ui->fromSpinBox->value() - 1;
		default:
			return 0;
	}
}

int ExportDialog::to() const
{
	switch(selectionMode())
	{
		case AllSlides:
			return ui->toSpinBox->maximum() - 1;
		case CustomRange:
			return ui->toSpinBox->value() - 1;
		default:
			return 0;
	}
}

void ExportDialog::validate()
{
	if(QDir(directory()).entryList(QDir::Files).count() > 0)
	{
		const int result = QMessageBox::question(this, this->windowTitle(), tr("Le dossier sélectionné n'est pas vide. Désirez-vous poursuivre l'exportation dans ce dossier ?<br />Les fichiers existants seront écrasés."), QMessageBox::Yes | QMessageBox::Cancel);
		if(result == QMessageBox::Cancel)
			return;
	}

	this->accept();
}

void ExportDialog::enableOkButton()
{
	ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(
		ui->directoryLineEdit->text().isEmpty() ||
		ui->templateLineEdit->text().isEmpty()
	);
}

void ExportDialog::on_toolButton_clicked()
{
	const QString directory = QFileDialog::getExistingDirectory(this, this->windowTitle());
	if(directory.isEmpty())
		return;

	ui->directoryLineEdit->setText(directory);
	enableOkButton();
}

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

#include "importdialog.h"
#include "ui_importdialog.h"

ImportDialog::ImportDialog(int slideCount, QWidget *parent) : QDialog(parent), ui(new Ui::ImportDialog)
{
	ui->setupUi(this);
	this->slideCount = slideCount;
	this->previousFilter = this->previousSort = this->modified = 0;

	ui->splitter->setStretchFactor(0, 100);

	ui->filterComboBox->blockSignals(true);
	ui->filterComboBox->addItem(tr("Mixte"),
		QStringList()
			<< parseFilter(IMAGE_FILTER)
			<< parseFilter(MOVIE_FILTER)
			<< parseFilter(AUDIO_FILTER)
	);
	ui->filterComboBox->addItem(tr("Images"), parseFilter(IMAGE_FILTER));
	ui->filterComboBox->addItem(tr("Vidéos"), parseFilter(MOVIE_FILTER));
	ui->filterComboBox->addItem(tr("Audio"), parseFilter(AUDIO_FILTER));
	ui->filterComboBox->blockSignals(false);

	ui->sortComboBox->blockSignals(true);
	ui->sortComboBox->addItem(tr("Nom"), QDir::Name);
	ui->sortComboBox->addItem(tr("Nom, inversé"), QDir::Reversed);
	ui->sortComboBox->addItem(tr("Date"), QDir::Time);
	ui->sortComboBox->addItem(tr("Taille"), QDir::Size);
	ui->sortComboBox->addItem(tr("Type"), QDir::Type);
	ui->sortComboBox->blockSignals(false);

	bool showProperties = QSettings().value("importDialog/showProperties").toBool();
	ui->propertiesButton->setChecked(showProperties);
	on_propertiesButton_toggled(showProperties);

	ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}

ImportDialog::~ImportDialog()
{
	garbageCollector(this->result() != QDialog::Accepted);
	delete ui;
}

QList<Slide *> ImportDialog::slides()
{
	QList<Slide *> slides;
	const int slidesCount = ui->treeWidget->topLevelItemCount();
	for(int index = 0; index < slidesCount; index++)
	{
		QTreeWidgetItem *slideItem = ui->treeWidget->topLevelItem(index);
		Slide *slide = (Slide *)slideItem->data(1, Qt::UserRole).value<void *>();

		const int elementCount = slideItem->childCount();
		for(int elementIndex = 0; elementIndex < elementCount; elementIndex++)
		{
			QTreeWidgetItem *elementItem = slideItem->child(elementIndex);
			if(elementItem->checkState(0) != Qt::Checked)
				continue;

			SlideElement *element = (SlideElement *)elementItem->data(1, Qt::UserRole).value<void *>();
			slide->addElement(element);
		}

		if(!slide->getElements().empty())
			slides.append(slide);
	}
	return slides;
}

const QStringList ImportDialog::parseFilter(QString filter)
{
	return filter.remove(QRegExp("(^[^\\(]+\\(|\\)[^\\)]*$)"))
				 .split('\x20', QString::SkipEmptyParts);
}

bool ImportDialog::updateList(const QString directory)
{
	if(modified)
	{
		if(QMessageBox::Cancel == QMessageBox::warning(this, tr("Regénérer la liste"), tr("Les modifications de la liste d'importations seront perdus. Voulez-vous continuer ?"), QMessageBox::Yes, QMessageBox::Cancel))
			return false;
	}

	garbageCollector();
	ui->treeWidget->blockSignals(true);
	ui->treeWidget->clear();

	int slideCount = this->slideCount;
	const QStringList filters = ui->filterComboBox->itemData(ui->filterComboBox->currentIndex()).toStringList();
	QFlags<QDir::SortFlag> sortBy = QDir::NoSort;
	switch(ui->sortComboBox->itemData(ui->sortComboBox->currentIndex()).toInt())
	{
		case QDir::Name:
			sortBy = QDir::Name;
			break;
		case QDir::Reversed:
			sortBy = QDir::Name | QDir::Reversed;
			break;
		case QDir::Time:
			sortBy = QDir::Time | QDir::Reversed;
			break;
		case QDir::Size:
			sortBy = QDir::Size;
			break;
		case QDir::Type:
			sortBy = QDir::Type;
			break;
	}

	QDir dir(directory);
	QStringList files = dir.entryList(filters, QDir::Files | QDir::Readable, sortBy);
	foreach(QString file, files)
	{
		Slide *slide = new Slide(tr("Diapositive %1").arg(++slideCount));
		connect(slide, SIGNAL(modified()), this, SLOT(elementModified()));

		SlideElement *element = createElementFor(dir.absoluteFilePath(file));
		connect(element, SIGNAL(modified()), this, SLOT(elementModified()));
		element->setValue("name", file);

		QTreeWidgetItem *slideItem = new QTreeWidgetItem(ui->treeWidget);
		slideItem->setText(0, slide->getValue("name").toString());
		slideItem->setData(0, Qt::UserRole, slideItem->text(0));
		slideItem->setData(1, Qt::UserRole, QVariant::fromValue<void *>((void *)slide));
		slideItem->setFlags(slideItem->flags() | Qt::ItemIsEditable);
		slideItem->setFlags(slideItem->flags() ^ Qt::ItemIsDragEnabled);
		slideItem->setExpanded(true);

		QTreeWidgetItem *fileItem = new QTreeWidgetItem(slideItem);
		fileItem->setText(0, element->getValue("name").toString());
		fileItem->setIcon(0, getIconFor(file));
		//fileItem->setData(0, Qt::UserRole, file);
		fileItem->setData(1, Qt::UserRole, QVariant::fromValue<void *>((void *)element));
		fileItem->setFlags(fileItem->flags() | Qt::ItemIsUserCheckable);
		fileItem->setFlags(fileItem->flags() ^ Qt::ItemIsDropEnabled);
		fileItem->setToolTip(0, dir.absoluteFilePath(file));
		fileItem->setCheckState(0, Qt::Checked);
	}

	if(files.isEmpty())
	{
		QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget);
		item->setText(0, tr("Aucun fichier ne correspond aux critères demandés dans ce dossier."));
		item->setFlags(Qt::NoItemFlags);
	}

	on_treeWidget_itemSelectionChanged();
	ui->treeWidget->blockSignals(false);
	this->modified = false;

	return true;
}

ImportDialog::ElementType ImportDialog::typeOf(const QString file)
{
	const QString extension = QString("*.%1").arg(QFileInfo(file).suffix().toLower());
	if(parseFilter(IMAGE_FILTER).contains(extension))
		return ImageType;
	else if(parseFilter(MOVIE_FILTER).contains(extension))
		return MovieType;
	else if(parseFilter(AUDIO_FILTER).contains(extension))
		return AudioType;

	return NullType;
}

SlideElement *ImportDialog::createElementFor(const QString file)
{
	const int type = typeOf(file);
	SlideElement *element = 0;
	switch(type)
	{
		case ImageType:
			element = new ImageElement;
			element->setValue("name", QFileInfo(file).baseName());
			element->setValue("src", file);
			element->setValue("size", QPixmap(file).size());
			break;
		case MovieType:
			element = new MovieElement;
			element->setValue("name", QFileInfo(file).baseName());
			element->setValue("src", file);
			element->setValue("size", QDesktopWidget().screenGeometry());  // TODO: use movie size
			break;
		case AudioType:
			element = new AudioElement;
			element->setValue("name", QFileInfo(file).baseName());
			element->setValue("src", file);
			break;
	}
	return element;
}

QIcon ImportDialog::getIconFor(const QString file)
{
	const int type = typeOf(file);
	switch(type)
	{
		case ImageType:
			return QIcon::fromTheme("image-x-generic");
		case MovieType:
			return QIcon::fromTheme("video-x-generic");
		case AudioType:
			return QIcon::fromTheme("audio-x-generic");
	}
	return QIcon();
}

void ImportDialog::garbageCollector(bool destroyEnabledElements)
{
	const int slidesCount = ui->treeWidget->topLevelItemCount();
	for(int index = 0; index < slidesCount; index++)
	{
		QTreeWidgetItem *slideItem = ui->treeWidget->topLevelItem(index);
		const int elementsCount = slideItem->childCount();
		int enabledElements = 0;
		for(int index = 0; index < elementsCount; index++)
		{
			QTreeWidgetItem *elementItem = slideItem->child(index);
			if(elementItem->checkState(0) == Qt::Checked)
				enabledElements++;

			if(destroyEnabledElements || elementItem->checkState(0) == Qt::Unchecked)
				delete (SlideElement *)elementItem->data(1, Qt::UserRole).value<void *>();
		}

		if(destroyEnabledElements || enabledElements == 0)
			delete (Slide *)slideItem->data(1, Qt::UserRole).value<void *>();
	}
}

void ImportDialog::enableOkButton()
{
	ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(
		ui->treeWidget->topLevelItemCount() < 1
	);
}

void ImportDialog::elementModified()
{
	this->modified = true;

	ui->treeWidget->blockSignals(true);
	QTreeWidgetItem *item = ui->treeWidget->selectedItems()[0];
	if(item->parent() == 0)
	{
		BaseElement *element = (BaseElement *)sender();
		item->setText(0, element->getValue("name").toString());
		item->setData(0, Qt::UserRole, element->getValue("name").toString());
	}
	ui->treeWidget->blockSignals(false);
}

void ImportDialog::on_directoryButton_clicked()
{
	const QString directory = QFileDialog::getExistingDirectory(this, this->windowTitle());
	if(directory.isEmpty())
		return;

	if(updateList(directory))
	{
		ui->directoryButton->setText(directory);
		ui->filterComboBox->setEnabled(true);
		ui->sortComboBox->setEnabled(true);
		enableOkButton();
	}
}

void ImportDialog::on_filterComboBox_currentIndexChanged(int index)
{
	if(!updateList(ui->directoryButton->text()))
	{
		ui->filterComboBox->blockSignals(true);
		ui->filterComboBox->setCurrentIndex(previousFilter);
		ui->filterComboBox->blockSignals(false);
		return;
	}

	previousFilter = index;
}

void ImportDialog::on_sortComboBox_currentIndexChanged(int index)
{
	if(!updateList(ui->directoryButton->text()))
	{
		ui->sortComboBox->blockSignals(true);
		ui->sortComboBox->setCurrentIndex(previousSort);
		ui->sortComboBox->blockSignals(false);
		return;
	}

	previousSort = index;
}

void ImportDialog::on_treeWidget_itemChanged(QTreeWidgetItem *item, int)
{
	this->modified = true;
	if(item->parent() == 0)
	{
		if(!MainWindow::validateSlideName(item->text(0)))
		{
			return item->setText(0, item->data(0, Qt::UserRole).toString());
		}

		item->setData(0, Qt::UserRole, item->text(0));
		BaseElement *element = (BaseElement *)item->data(1, Qt::UserRole).value<void *>();
		element->setValue("name", item->text(0));
		on_treeWidget_itemSelectionChanged();
	}
}

void ImportDialog::on_propertiesButton_toggled(bool checked)
{
	ui->propertiesButton->setText(tr("Propriétés %1").arg(checked ? "<<" : ">>"));
	ui->propertiesEditor->setVisible(checked);

	QSettings().setValue("importDialog/showProperties", checked);
}

void ImportDialog::on_treeWidget_itemSelectionChanged()
{
	foreach(QtProperty *prop, ui->propertiesEditor->properties())
	{
		ui->propertiesEditor->unsetFactoryForManager(prop->propertyManager());
		prop->propertyManager()->deleteLater();
	}
	ui->propertiesEditor->clear();

	if(ui->treeWidget->selectedItems().empty())
		return;

	BaseElement *element = (BaseElement *)ui->treeWidget->currentItem()->data(1, Qt::UserRole).value<void *>();
	element->bindProperties(ui->propertiesEditor);
}

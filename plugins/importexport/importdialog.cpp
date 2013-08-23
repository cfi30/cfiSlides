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
#include <QMessageBox>
#include <QTreeWidgetItem>
#include <QSettings>
#include <QFileInfo>
#include <QInputDialog>
#include <QProgressDialog>
#include <QDesktopWidget>
#include <QDropEvent>

#include "importdialog.h"
#include "ui_importdialog.h"
#include "slide.h"
#include "slideshow.h"
#include "slideelement.h"
#include "icon_t.h"
#include "configuration.h"

ImportDialog::ImportDialog(const int slideCount, const Slideshow *slideshow, QWidget *parent) : QDialog(parent), ui(new Ui::ImportDialog)
{
	ui->setupUi(this);

	this->slideCount = slideCount;
	this->slideshow = const_cast<Slideshow *>(slideshow);
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

	bool showProperties = QSettings().value(QStringLiteral("importDialog/showProperties")).toBool();
	ui->propertiesButton->setChecked(showProperties);
	on_propertiesButton_toggled(showProperties);

	ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}

ImportDialog::~ImportDialog()
{
	garbageCollector();
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
			slide->addElement(element->clone());
		}

		if(!slide->getElements().empty())
			slides << slide;
	}

	return slides;
}

const QStringList ImportDialog::parseFilter(QString filter) const
{
	return filter.remove(QRegExp(QStringLiteral("(^[^\\(]+\\(|\\)[^\\)]*$)")))
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

	QProgressDialog *progress = new QProgressDialog(this);
	progress->setWindowTitle(this->windowTitle());
	progress->setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
	progress->setCancelButtonText(QString());
	progress->setLabelText(tr("Recherche en cours des fichiers correspondants..."));
	progress->open();

	QDir dir(directory);
	QStringList files = dir.entryList(filters, QDir::Files | QDir::Readable, sortBy);

	const int fileCount = files.size();
	progress->setMaximum(fileCount);

	for(int index = 0; index < fileCount; index++)
	{
		QString file = files[index];
		progress->setValue(index + 1);

		Slide *slide = new Slide(slideshow);
		slide->setValue(QStringLiteral("name"), tr("Diapositive %1").arg(++slideCount));
		connect(slide, &Slide::modified, this, &ImportDialog::elementModified);

		SlideElement *element = createElementFor(dir.absoluteFilePath(file));
		connect(element, &Slide::modified, this, &ImportDialog::elementModified);
		element->setValue(QStringLiteral("name"), QFileInfo(file).baseName());
		element->setSlide(slide);

		QTreeWidgetItem *slideItem = new QTreeWidgetItem(ui->treeWidget);
		slideItem->setText(0, slide->getValue(QStringLiteral("name")).toString());
		slideItem->setData(0, Qt::UserRole, slideItem->text(0));
		slideItem->setData(1, Qt::UserRole, QVariant::fromValue<void *>((void *)slide));
		slideItem->setFlags(slideItem->flags() | Qt::ItemIsEditable);
		slideItem->setExpanded(true);

		QTreeWidgetItem *fileItem = new QTreeWidgetItem(slideItem);
		fileItem->setText(0, element->getValue(QStringLiteral("name")).toString());
		fileItem->setIcon(0, getIconFor(file));
		fileItem->setData(1, Qt::UserRole, QVariant::fromValue<void *>((void *)element));
		fileItem->setFlags(fileItem->flags() | Qt::ItemIsEditable | Qt::ItemIsUserCheckable);
		fileItem->setToolTip(0, dir.absoluteFilePath(file));
		fileItem->setCheckState(0, Qt::Checked);
	}

	if(files.isEmpty())
	{
		QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget);
		item->setText(0, tr("Aucun fichier ne correspond aux critères demandés dans ce dossier."));
		item->setFlags(Qt::NoItemFlags);
	}

	progress->close();
	on_treeWidget_itemSelectionChanged();
	ui->treeWidget->blockSignals(false);
	this->modified = false;

	return true;
}

ImportDialog::ElementType ImportDialog::typeOf(const QString &file) const
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

SlideElement *ImportDialog::createElementFor(const QString &file) const
{
	const int type = typeOf(file);
	SlideElement *element = 0;
	switch(type)
	{
		case ImageType:
			element = createElement("ImageElement");
			element->setValue(QStringLiteral("name"), QFileInfo(file).baseName());
			element->setValue(QStringLiteral("src"), file);
			element->setValue(QStringLiteral("size"), getImageSizeFor(file));
			break;
		case MovieType:
			element = createElement("VideoElement");
			element->setValue(QStringLiteral("name"), QFileInfo(file).baseName());
			element->setValue(QStringLiteral("src"), file);
			element->setValue(QStringLiteral("size"), QDesktopWidget().screenGeometry().size() / DEFAULT_SIZE_SCALE);  // TODO: use movie size
			break;
		case AudioType:
			element = createElement("AudioElement");
			element->setValue(QStringLiteral("name"), QFileInfo(file).baseName());
			element->setValue(QStringLiteral("src"), file);
			break;
	}
	return element;
}

SlideElement *ImportDialog::createElement(const char *type) const
{
	return (SlideElement *)QMetaType::create(QMetaType::type(type));
}

QIcon ImportDialog::getIconFor(const QString &file) const
{
	const int type = typeOf(file);
	switch(type)
	{
		case ImageType:
			return ICON_T("image-x-generic");
		case MovieType:
			return ICON_T("video-x-generic");
		case AudioType:
			return ICON_T("audio-x-generic");
	}
	return QIcon();
}

QSize ImportDialog::getImageSizeFor(const QString &file) const
{
	QSize size = QPixmap(file).size();
	const QSize sceneSize = slideshow->getValue(QStringLiteral("size")).toSize();
	if(size.width() > sceneSize.width() || size.height() > sceneSize.height())
		size.scale(sceneSize, Qt::KeepAspectRatio);

	return size;
}

void ImportDialog::garbageCollector() const
{
	const int slidesCount = ui->treeWidget->topLevelItemCount();
	for(int index = 0; index < slidesCount; index++)
	{
		bool deleteSlide = true;

		QTreeWidgetItem *slideItem = ui->treeWidget->topLevelItem(index);
		const int elementCount = slideItem->childCount();
		for(int elementIndex = 0; elementIndex < elementCount; elementIndex++)
		{
			const QTreeWidgetItem *elementItem = slideItem->child(elementIndex);
			if(elementItem->checkState(0) == Qt::Checked)
				deleteSlide = false;

			delete (SlideElement *)elementItem->data(1, Qt::UserRole).value<void *>();
		}

		if(deleteSlide) delete (Slide *)slideItem->data(1, Qt::UserRole).value<void *>();
	}
}

void ImportDialog::enableOkButton()
{
	ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(
		ui->treeWidget->topLevelItemCount() > 0 && !ui->treeWidget->topLevelItem(0)->flags().testFlag(Qt::NoItemFlags)
	);
}

void ImportDialog::elementModified()
{
	this->modified = true;

	ui->treeWidget->blockSignals(true);
	QTreeWidgetItem *item = ui->treeWidget->selectedItems()[0];
	if(item->parent() == 0)
	{
		SlideshowElement *element = (SlideshowElement *)sender();
		item->setText(0, element->getValue(QStringLiteral("name")).toString());
		item->setData(0, Qt::UserRole, element->getValue(QStringLiteral("name")).toString());
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
	if(item->text(0).trimmed().isEmpty())
		return item->setText(0, item->data(0, Qt::UserRole).toString());

	item->setData(0, Qt::UserRole, item->text(0));
	SlideshowElement *element = (SlideshowElement *)item->data(1, Qt::UserRole).value<void *>();
	element->setValue(QStringLiteral("name"), item->text(0));
	on_treeWidget_itemSelectionChanged();
}

void ImportDialog::on_propertiesButton_toggled(bool checked)
{
	ui->propertiesButton->setText(tr("Propriétés %1").arg(checked ? "<<" : ">>"));
	ui->sidePanel->setVisible(checked);

	QSettings().setValue(QStringLiteral("importDialog/showProperties"), checked);
}

void ImportDialog::on_treeWidget_itemSelectionChanged()
{
	ui->propertiesEditor->clear();

	const bool selectionIsEmpty = ui->treeWidget->selectedItems().empty();
	ui->sidePanel->setDisabled(selectionIsEmpty);

	if(!selectionIsEmpty)
	{
		SlideshowElement *element = (SlideshowElement *)ui->treeWidget->currentItem()->data(1, Qt::UserRole).value<void *>();
		ui->propertiesEditor->addTopLevelProperties(element->getProperties());
	}
}

void ImportDialog::on_moveUpButton_clicked()
{
	QTreeWidgetItem *item = ui->treeWidget->currentItem();
	QTreeWidgetItem *parent = item->parent();

	if(!parent)
	{
		const int currentIndex = ui->treeWidget->indexOfTopLevelItem(item);
		if(currentIndex == 0) return;

		this->modified = true;

		ui->treeWidget->takeTopLevelItem(currentIndex);
		ui->treeWidget->insertTopLevelItem(currentIndex - 1, item);
		item->setExpanded(true);
	}
	else
	{
		const int currentIndex = parent->indexOfChild(item);
		const int parentIndex = ui->treeWidget->indexOfTopLevelItem(parent);

		if(currentIndex + parentIndex > 0)
		{
			parent->takeChild(currentIndex);
			this->modified = true;
		}

		if(currentIndex > 0)
			parent->insertChild(currentIndex - 1, item);
		else if(parentIndex > 0)
		{
			QTreeWidgetItem *parentNeighbour = ui->treeWidget->topLevelItem(parentIndex - 1);
			parentNeighbour->addChild(item);
		}
	}

	ui->treeWidget->setCurrentItem(item);
}

void ImportDialog::on_moveDownButton_clicked()
{
	QTreeWidgetItem *item = ui->treeWidget->currentItem();
	QTreeWidgetItem *parent = item->parent();
	const int lastParentIndex = ui->treeWidget->topLevelItemCount() - 1;

	if(!parent)
	{
		const int currentIndex = ui->treeWidget->indexOfTopLevelItem(item);
		if(currentIndex == lastParentIndex) return;

		this->modified = true;

		ui->treeWidget->takeTopLevelItem(currentIndex);
		ui->treeWidget->insertTopLevelItem(currentIndex + 1, item);
		item->setExpanded(true);
	}
	else
	{
		const int currentIndex = parent->indexOfChild(item);
		const int lastBrotherIndex = parent->childCount() - 1;
		const int parentIndex = ui->treeWidget->indexOfTopLevelItem(parent);

		if(currentIndex < lastBrotherIndex)
		{
			parent->takeChild(currentIndex);
			parent->insertChild(currentIndex + 1, item);
		}
		else if(parentIndex < lastParentIndex)
		{
			QTreeWidgetItem *parentNeighbour = ui->treeWidget->topLevelItem(parentIndex + 1);
			parent->takeChild(currentIndex);
			parentNeighbour->insertChild(0, item);
		}
		else { return; }

		this->modified = true;
	}

	ui->treeWidget->setCurrentItem(item);
}

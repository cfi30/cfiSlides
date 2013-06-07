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

#include "imageelement.h"

ImageElement::ImageElement() : SlideElement()
{
	setValue("size", QSize(600, 400));
}

void ImageElement::render(QGraphicsScene *scene, const bool interactive)
{
	SlideElement::render(scene, interactive);

	if(!getValue("visible").toBool())
		return;

	Qt::AspectRatioMode scalingMode = Qt::IgnoreAspectRatio;
	switch(getValue("scaleMode").toInt())
	{
		case 1:
			scalingMode = Qt::KeepAspectRatio;
			break;
		case 2:
			scalingMode = Qt::KeepAspectRatioByExpanding;
			break;
	}

	if(!getValue("src").toString().isEmpty())
	{
		QPixmap pixmap(getValue("src").toString());
		QPixmap spixmap = pixmap.scaled(getValue("size").toSize(), scalingMode, Qt::SmoothTransformation);

		GraphicsPixmapItem *item = new GraphicsPixmapItem();
		item->setPixmap(spixmap);
		item->setPos(getValue("position").toPoint());
		item->setElement(this);

		scene->addItem(item);
	}
	else
	{
		GraphicsMoviePreviewItem *item = new GraphicsMoviePreviewItem; // FIXME: should display an image placeholder
		item->setSize(getValue("size").toSize());
		item->setPos(getValue("position").toPoint());
		item->setElement(this);

		scene->addItem(item);
	}
}

PropertyList ImageElement::getProperties() const
{
	FilePropertyManager *fileManager = new FilePropertyManager;
	connect(fileManager, SIGNAL(modified(QString, QVariant)), this, SLOT(propertyChanged(QString, QVariant)));

	EnumPropertyManager *enumManager = new EnumPropertyManager;
	connect(enumManager, SIGNAL(modified(QString, QVariant)), this, SLOT(propertyChanged(QString, QVariant)));

	Property *group = new Property(0, tr("Image"));

	Property *src = new Property(fileManager, tr("Source"), "src");
	src->setToolTip(tr("Chemin de l'image"));
	src->setValue(this->getValue("src"));
	fileManager->setRequired("src", true);
	fileManager->setFilter("src", IMAGE_FILTER);
	group->addProperty(src);

	Property *scaleMode = new Property(enumManager, tr("Mise à l'échelle"), "scaleMode");
	scaleMode->setToolTip(tr("Mode de mise à l'échelle de l'image"));
	scaleMode->setValue(this->getValue("scaleMode"));
	enumManager->setEnumNames("scaleMode", QStringList() << tr("Remplir") << tr("Conserver") << tr("Remplir & Conserver"));
	group->addProperty(scaleMode);

	return PropertyList()
		<< SlideElement::getProperties()
		<< group;
}

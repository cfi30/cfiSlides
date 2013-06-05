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

void ImageElement::bindProperties(QtTreePropertyBrowser *browser) const
{
	SlideElement::bindProperties(browser);

	QtGroupPropertyManager *groupManager = new QtGroupPropertyManager();
	FilePathManager *filePathManager = new FilePathManager();
	FileEditFactory *fileEditFactory = new FileEditFactory(browser);
	QtEnumPropertyManager *enumManager = new QtEnumPropertyManager();
	QtEnumEditorFactory *enumEditorFactory = new QtEnumEditorFactory(browser);

	QtProperty *group = groupManager->addProperty(tr("Image"));
	group->setModified(true);

	QtProperty *src = filePathManager->addProperty(tr("Source"));
	src->setWhatsThis("src");
	src->setToolTip(tr("Chemin de l'image"));
	filePathManager->setValue(src, getValue("src").toString());
	filePathManager->setFilter(src, IMAGE_FILTER);
	group->addSubProperty(src);

	QtProperty *scalingMode = enumManager->addProperty(tr("Mise à l'échelle"));
	scalingMode->setWhatsThis("scaleMode");
	scalingMode->setToolTip(tr("Mode de mise à l'échelle de l'image"));
	enumManager->setEnumNames(scalingMode, QStringList() << tr("Remplir") << tr("Conserver") << tr("Remplir & Conserver"));
	enumManager->setValue(scalingMode, getValue("scaleMode").toInt());
	group->addSubProperty(scalingMode);

	browser->addProperty(group);

	browser->setFactoryForManager(filePathManager, fileEditFactory);
	browser->setFactoryForManager(enumManager, enumEditorFactory);

	connect(filePathManager, SIGNAL(valueChanged(QtProperty*,QString)), this, SLOT(stringValueChanged(QtProperty*,QString)));
	connect(enumManager, SIGNAL(valueChanged(QtProperty*,int)), this, SLOT(intValueChanged(QtProperty*,int)));
}

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
#include "slideshow.h"

ImageElement::ImageElement() : SlideElement()
{
	setValue(QStringLiteral("size"), QSize(400, 300));
}

QGraphicsItem *ImageElement::render(const bool interactive)
{
	if(!getValue(QStringLiteral("visible")).toBool())
		return 0;

	const QSize size = getValue(QStringLiteral("size")).toSize();
	const QPoint pos = getValue(QStringLiteral("position")).toPoint();

	const QPixmap pixmap(getValue(QStringLiteral("src")).toString());
	if(pixmap.isNull())
	{
		MissingImagePlaceholderItem *item = new MissingImagePlaceholderItem(interactive, this);
		item->setPos(pos);
		item->setRect(QRect(QPoint(), size));
		item->setBrush(Qt::darkGray);
		item->setPen(QPen(Qt::black));

		QGraphicsPixmapItem *icon = new QGraphicsPixmapItem(item);
		icon->setPixmap(QIcon::fromTheme("image-missing").pixmap(QSize(128, 128)));

		icon->setVisible(
			size.width() > icon->pixmap().size().width() &&
			size.height() > icon->pixmap().size().height()
		);
		icon->setPos(
			(size.width() - icon->pixmap().width()) / 2,
			(size.height() - icon->pixmap().height()) / 2
		);

		return item;
	}
	else
	{
		const QPixmap spixmap = pixmap.scaled(size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

		GraphicsPixmapItem *item = new GraphicsPixmapItem(interactive, this);
		item->setPixmap(spixmap);
		item->setPos(getValue(QStringLiteral("position")).toPoint());

		return item;
	}
}

PropertyList ImageElement::getProperties() const
{
	FilePropertyManager *fileManager = new FilePropertyManager;
	connect(fileManager, &PropertyManager::modified, this, &ImageElement::propertyChanged);

	Property *group = new Property(0, tr("Image"));

	Property *src = new Property(fileManager, tr("Source"), QStringLiteral("src"));
	src->setToolTip(tr("Chemin de l'image"));
	src->setValue(this->getValue(QStringLiteral("src")));
	fileManager->setRequired(QStringLiteral("src"), true);
	fileManager->setFilter(QStringLiteral("src"), IMAGE_FILTER);
	group->addProperty(src);

	return PropertyList()
		<< SlideElement::getProperties()
		<< group;
}

void ImageElement::propertyChanged(const QString &name, const QVariant &value)
{
	if(getValue(QStringLiteral("src")).toString().isEmpty())
	{
		QSize size = QPixmap(value.toString()).size();
		if(!size.isNull())
		{
			const QSize sceneSize = slideshow()->getValue(QStringLiteral("geometry")).toRect().size();
			if(size.width() > sceneSize.width() || size.height() > sceneSize.height())
				size.scale(sceneSize, Qt::KeepAspectRatio);

			setValue(QStringLiteral("size"), size);
			emit updateProperties();
		}
	}

	SlideshowElement::propertyChanged(name, value);
}

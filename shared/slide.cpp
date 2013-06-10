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

#include "slide.h"

Slide::Slide(const QString &name) : SlideshowElement()
{
	this->setValue("name", name);
	this->setValue("backgroundColor", QColor(Qt::white));
}

Slide::~Slide()
{
	while(!this->elements.isEmpty())
		delete this->elements.takeFirst();
}

void Slide::render(QGraphicsScene *scene, const bool interactive)
{
	QBrush background;
	QPixmap backgroundPixmap(this->getValue("backgroundImage").toString());
	if(backgroundPixmap.isNull())
	{
		background.setColor(getValue("backgroundColor").value<QColor>());
		background.setStyle(Qt::SolidPattern);
	}
	else
	{
		switch(getValue("backgroundImageStretch").toInt())
		{
			case Slide::Repeat:
				break;
			case Slide::IgnoreRatio:
				backgroundPixmap = backgroundPixmap.scaled(scene->sceneRect().width(), scene->sceneRect().height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
				break;
			case Slide::Fill:
				backgroundPixmap = backgroundPixmap.scaled(scene->sceneRect().width(), scene->sceneRect().height(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
				break;
			case Slide::KeepRatio:
				backgroundPixmap = backgroundPixmap.scaled(scene->sceneRect().width(), scene->sceneRect().height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
				break;
		}
		background.setTexture(backgroundPixmap);
	}

	scene->addRect(scene->sceneRect(), QPen(Qt::NoPen), background);

	foreach(SlideElement *element, this->elements)
		element->render(scene, interactive);

	if(interactive)
	{
		foreach(QGraphicsItem *item, scene->items())
		{
			if(item->data(Qt::UserRole).isValid())
				item->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemSendsGeometryChanges);
		}
	}
}

QList<SlideElement *> Slide::getElements() const
{
	return this->elements;
}

SlideElement *Slide::getElement(const int index) const
{
	return this->elements[index];
}

void Slide::addElement(SlideElement *element)
{
	connect(element, SIGNAL(modified()), this, SLOT(elementChanged()));
	connect(element, SIGNAL(moved()), this, SLOT(elementMoved()));
	connect(element, SIGNAL(refresh()), this, SLOT(refreshRequest()));
	element->setIndex(this->elements.count());
	this->elements.append(element);
}

void Slide::removeElement(const int index)
{
	delete getElement(index);
	this->elements.removeAt(index);

	int elementsCount = this->elements.count();
	for(int i = 0; i < elementsCount; i++)
		this->elements[i]->setIndex(i);
}

void Slide::moveElement(const int from, const int to)
{
	this->elements.move(from, to);

	int elementsCount = this->elements.count();
	for(int i = 0; i < elementsCount; i++)
		this->elements[i]->setIndex(i);
}

PropertyList Slide::getProperties() const
{
	ColorPropertyManager *colorManager = new ColorPropertyManager;
	connect(colorManager, SIGNAL(modified(QString, QVariant)), this, SLOT(propertyChanged(QString, QVariant)));

	FilePropertyManager *fileManager = new FilePropertyManager;
	connect(fileManager, SIGNAL(modified(QString, QVariant)), this, SLOT(propertyChanged(QString, QVariant)));

	EnumPropertyManager *enumManager = new EnumPropertyManager;
	connect(enumManager, SIGNAL(modified(QString, QVariant)), this, SLOT(propertyChanged(QString, QVariant)));

	Property *background = new Property(0, tr("Arrière-plan"));

	Property *color = new Property(colorManager, tr("Couleur"), "backgroundColor");
	color->setValue(this->getValue("backgroundColor"));
	color->setToolTip(tr("Couleur de fond"));
	background->addProperty(color);

	Property *image = new Property(fileManager, tr("Image"), "backgroundImage");
	image->setValue(this->getValue("backgroundImage"));
	image->setToolTip(tr("Image de fond"));
	fileManager->setFilter("backgroundImage", IMAGE_FILTER);
	background->addProperty(image);

	Property *stretchMode = new Property(enumManager, tr("Mise à l'échelle"), "backgroundImageStretch");
	stretchMode->setValue(this->getValue("backgroundImageStretch"));
	stretchMode->setToolTip(tr("Mode de mise à l'échelle de l'image"));
	enumManager->setEnumNames("backgroundImageStretch", QStringList() << tr("Remplir & Conserver") << tr("Répéter") << tr("Conserver"));
	image->addProperty(stretchMode);

	return PropertyList()
		<< SlideshowElement::getProperties()
		<< background;
}

void Slide::elementChanged()
{
	emit modified();
}

void Slide::elementMoved()
{
	emit moved();
}

void Slide::refreshRequest()
{
	emit refresh();
}

void Slide::play()
{
	foreach(SlideElement *element, elements)
	{
		if(element->getValue("visible").toBool())
			element->play();
	}
}

void Slide::pause()
{
	foreach(SlideElement *element, elements)
	{
		if(element->getValue("visible").toBool())
			element->pause();
	}
}

void Slide::stop()
{
	foreach(SlideElement *element, elements)
	{
		if(element->getValue("visible").toBool())
			element->stop();
	}
}

void Slide::toggleMute()
{
	foreach(SlideElement *element, elements)
	{
		if(element->getValue("visible").toBool())
			element->toggleMute();
	}
}

void Slide::destroy()
{
	foreach(SlideElement *element, elements)
	{
		if(element->getValue("visible").toBool())
			element->destroy();
	}
}

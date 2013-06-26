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

Slide::Slide(Slideshow *slideshow) : SlideshowElement()
{
	parentSlideshow = slideshow;
	setValue(QStringLiteral("name"), tr("Sans Nom"));
	setValue(QStringLiteral("backgroundColor"), QColor(Qt::white));
}

Slide::~Slide()
{
	while(!elements.isEmpty())
		delete elements.takeFirst();
}

void Slide::render(QGraphicsScene *scene, const bool interactive)
{
	QBrush background;
	background.setColor(getValue(QStringLiteral("backgroundColor")).value<QColor>());
	background.setStyle(Qt::SolidPattern);

	QPixmap backgroundPixmap(this->getValue(QStringLiteral("backgroundImage")).toString());
	if(!backgroundPixmap.isNull())
	{
		switch(getValue(QStringLiteral("backgroundImageStretch")).toInt())
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
	{
		QGraphicsItem *item = element->render(interactive);
		if(!item) continue;

		scene->addItem(item);
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
	element->setIndex(this->elements.count());
	element->setSlide(this);

	connect(element, &SlideshowElement::modified, this, &Slide::elementChanged);
	connect(element, &SlideElement::moved, this, &Slide::elementMoved);
	connect(element, &SlideElement::refresh, this, &Slide::refreshRequested);
	connect(element, &SlideElement::updateProperties, this, &Slide::updatePropertiesRequested);

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
	connect(colorManager, &PropertyManager::modified, this, &Slide::propertyChanged);

	FilePropertyManager *fileManager = new FilePropertyManager;
	connect(fileManager, &PropertyManager::modified, this, &Slide::propertyChanged);

	EnumPropertyManager *enumManager = new EnumPropertyManager;
	connect(enumManager, &PropertyManager::modified, this, &Slide::propertyChanged);

	Property *background = new Property(0, tr("Arrière-plan"));

	Property *color = new Property(colorManager, tr("Couleur"), QStringLiteral("backgroundColor"));
	color->setValue(this->getValue(QStringLiteral("backgroundColor")));
	color->setToolTip(tr("Couleur de fond"));
	background->addProperty(color);

	Property *image = new Property(fileManager, tr("Image"), QStringLiteral("backgroundImage"));
	image->setValue(this->getValue(QStringLiteral("backgroundImage")));
	image->setToolTip(tr("Image de fond"));
	fileManager->setFilter(QStringLiteral("backgroundImage"), IMAGE_FILTER);
	background->addProperty(image);

	Property *stretchMode = new Property(enumManager, tr("Mise à l'échelle"), QStringLiteral("backgroundImageStretch"));
	stretchMode->setValue(this->getValue(QStringLiteral("backgroundImageStretch")));
	stretchMode->setToolTip(tr("Mode de mise à l'échelle de l'image"));
	enumManager->setEnumNames(QStringLiteral("backgroundImageStretch"), QStringList() << tr("Remplir & Conserver") << tr("Répéter") << tr("Conserver"));
	image->addProperty(stretchMode);

	return PropertyList()
		<< SlideshowElement::getProperties()
		<< background;
}

Slideshow *Slide::slideshow() const
{
	return parentSlideshow;
}

void Slide::setSlideshow(Slideshow *slideshow)
{
	parentSlideshow = slideshow;
}

void Slide::elementChanged()
{
	emit modified();
}

void Slide::elementMoved()
{
	emit moved();
}

void Slide::refreshRequested()
{
	emit refresh();
}

void Slide::updatePropertiesRequested()
{
	emit updateProperties();
}

void Slide::play()
{
	foreach(SlideElement *element, elements)
	{
		if(element->getValue(QStringLiteral("visible")).toBool())
			element->play();
	}
}

void Slide::pause()
{
	foreach(SlideElement *element, elements)
	{
		if(element->getValue(QStringLiteral("visible")).toBool())
			element->pause();
	}
}

void Slide::stop()
{
	foreach(SlideElement *element, elements)
	{
		if(element->getValue(QStringLiteral("visible")).toBool())
			element->stop();
	}
}

void Slide::toggleMute()
{
	foreach(SlideElement *element, elements)
	{
		if(element->getValue(QStringLiteral("visible")).toBool())
			element->toggleMute();
	}
}

void Slide::destroy()
{
	foreach(SlideElement *element, elements)
	{
		if(element->getValue(QStringLiteral("visible")).toBool())
			element->destroy();
	}
}

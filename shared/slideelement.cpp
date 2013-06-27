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

#include <QRect>

#include "slideelement.h"
#include "slide.h"
#include "slideshow.h"
#include "propertymanager.h"
#include "configuration.h"

SlideElement::SlideElement() : SlideshowElement()
{
	parentSlide = 0;
	setValue(QStringLiteral("visible"), true);
}

SlideElement::SlideElement(const SlideElement &copy) : SlideshowElement()
{
	parentSlide = 0;
	setValues(copy.getValues());
}

const char *SlideElement::type() const
{
	return metaObject()->className();
}

QString SlideElement::previewUrl() const
{
	return QString();
}

PropertyList SlideElement::getProperties() const
{
	BoolPropertyManager *boolManager = new BoolPropertyManager;
	connect(boolManager, &PropertyManager::modified, this, &SlideElement::propertyChanged);

	SizePropertyManager *sizeManager = new SizePropertyManager;
	connect(sizeManager, &PropertyManager::modified, this, &SlideElement::propertyChanged);

	PointPropertyManager *pointManager = new PointPropertyManager;
	connect(pointManager, &PropertyManager::modified, this, &SlideElement::propertyChanged);

	Property *visible = new Property(boolManager, tr("Visible"), QStringLiteral("visible"));
	visible->setToolTip(tr("Visibilité de l'élément"));
	visible->setValue(this->getValue(QStringLiteral("visible")));

	Property *geometry = new Property(0, tr("Géométrie"));

	Property *position = new Property(pointManager, tr("Position"), QStringLiteral("position"));
	position->setToolTip(tr("Position de l'élément"));
	position->setValue(this->getValue(QStringLiteral("position")));
	geometry->addProperty(position);

	Property *size = new Property(sizeManager, tr("Taille"), QStringLiteral("size"));
	size->setToolTip(tr("Taille de l'élément"));
	size->setValue(getValue(QStringLiteral("size")));
	sizeManager->setMinimum(QStringLiteral("size"), MINIMUM_SIZE);
	sizeManager->setMaximum(QStringLiteral("size"), slideshow()->getValue(QStringLiteral("geometry")).toRect().size());
	geometry->addProperty(size);

	return PropertyList()
		<< SlideshowElement::getProperties()
		<< visible
		<< geometry;
}

void SlideElement::movedTo(QPoint pos)
{
	setValue(QStringLiteral("position"), pos);
	emit moved();
}

int SlideElement::getIndex() const
{
	return this->elementIndex;
}

void SlideElement::setIndex(const int newIndex)
{
	this->elementIndex = newIndex;
}

Slide *SlideElement::slide() const
{
	return parentSlide;
}

void SlideElement::setSlide(const Slide *slide)
{
	if(parentSlide)
		qFatal("this element is already attached to a slide");

	parentSlide = const_cast<Slide *>(slide);
}

Slideshow *SlideElement::slideshow() const
{
	return parentSlide->slideshow();
}

SlideElement *SlideElement::clone() const
{
	return (SlideElement *)QMetaType::create(QMetaType::type(type()), this);
}

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

#include "slideelement.h"

SlideElement::SlideElement() : SlideshowElement()
{
	this->scene = 0;
	setValue("visible", true);
}

SlideElement::SlideElement(const SlideElement &copy) : SlideshowElement()
{
	SlideElement();
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

void SlideElement::render(QGraphicsScene *scene, const bool interactive)
{
	if(interactive)
		this->scene = scene;
}

PropertyList SlideElement::getProperties() const
{
	BoolPropertyManager *boolManager = new BoolPropertyManager;
	connect(boolManager, &PropertyManager::modified, this, &SlideElement::propertyChanged);

	SizePropertyManager *sizeManager = new SizePropertyManager;
	connect(sizeManager, &PropertyManager::modified, this, &SlideElement::propertyChanged);

	PointPropertyManager *pointManager = new PointPropertyManager;
	connect(pointManager, &PropertyManager::modified, this, &SlideElement::propertyChanged);

	Property *visible = new Property(boolManager, tr("Visible"), "visible");
	visible->setToolTip(tr("Visibilité de l'élément"));
	visible->setValue(this->getValue("visible"));

	Property *geometry = new Property(0, tr("Géométrie"));

	Property *position = new Property(pointManager, tr("Position"), "position");
	position->setToolTip(tr("Position de l'élément"));
	position->setValue(this->getValue("position"));
	geometry->addProperty(position);

	Property *size = new Property(sizeManager, tr("Taille"), "size");
	size->setToolTip(tr("Taille de l'élément"));
	size->setValue(getValue("size"));
	sizeManager->setMinimum("size", MINIMUM_SIZE);
	if(this->scene != 0)
		sizeManager->setMaximum("size", scene->sceneRect().size().toSize());
	geometry->addProperty(size);

	return PropertyList()
		<< SlideshowElement::getProperties()
		<< visible
		<< geometry;
}

void SlideElement::movedTo(QPoint pos)
{
	setValue("position", pos);
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

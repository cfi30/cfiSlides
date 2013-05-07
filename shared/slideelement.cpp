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

SlideElement::SlideElement() : BaseElement()
{
	this->scene = 0;
	setValue("visible", true);
}

SlideElement::SlideElement(const SlideElement &copy) : BaseElement()
{
	SlideElement();
	setProperties(copy.getProperties());
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

void SlideElement::bindProperties(QtTreePropertyBrowser *browser) const
{
	BaseElement::bindProperties(browser);

	QtBoolPropertyManager *boolManager = new QtBoolPropertyManager();
	QtCheckBoxFactory *checkBoxFactory = new QtCheckBoxFactory(browser);
	QtGroupPropertyManager *groupManager = new QtGroupPropertyManager();
	QtSizePropertyManager *sizeManager = new QtSizePropertyManager();
	QtPointPropertyManager *pointManager = new QtPointPropertyManager();
	QtSpinBoxFactory *spinboxFactory = new QtSpinBoxFactory();

	QtProperty *visible = boolManager->addProperty(tr("Visible"));
	visible->setWhatsThis("visible");
	visible->setToolTip(tr("Visibilité de l'élément"));
	boolManager->setValue(visible, getValue("visible").toBool());
	browser->addProperty(visible);

	QtProperty *geometry = groupManager->addProperty(tr("Géométrie"));
	geometry->setModified(true);

	QtProperty *size = sizeManager->addProperty(tr("Taille"));
	size->setWhatsThis("size");
	size->setToolTip(tr("Taille de l'élément"));
	sizeManager->setValue(size, getValue("size").toSize());
	sizeManager->setMinimum(size, QSize(10, 10));
	if(this->scene != 0)
		sizeManager->setMaximum(size, scene->sceneRect().size().toSize());
	geometry->addSubProperty(size);

	QtProperty *position = pointManager->addProperty(tr("Position"));
	position->setWhatsThis("position");
	position->setToolTip(tr("Position de l'élément"));
	pointManager->setValue(position, getValue("position").toPoint());
	geometry->addSubProperty(position);

	browser->addProperty(geometry);

	foreach(QtProperty *prop, pointManager->subIntPropertyManager()->properties())
		pointManager->subIntPropertyManager()->setMinimum(prop, 0);

	browser->setFactoryForManager(boolManager, checkBoxFactory);
	browser->setFactoryForManager(sizeManager->subIntPropertyManager(), spinboxFactory);
	browser->setFactoryForManager(pointManager->subIntPropertyManager(), spinboxFactory);

	connect(boolManager, SIGNAL(valueChanged(QtProperty*,bool)), this, SLOT(boolValueChanged(QtProperty*,bool)));
	connect(sizeManager, SIGNAL(valueChanged(QtProperty*,QSize)), this, SLOT(sizeValueChanged(QtProperty*,QSize)));
	connect(pointManager, SIGNAL(valueChanged(QtProperty*,QPoint)), this, SLOT(pointValueChanged(QtProperty*,QPoint)));
}

void SlideElement::play()
{

}

void SlideElement::pause()
{

}

void SlideElement::stop()
{

}

void SlideElement::toggleMute()
{

}

void SlideElement::destroy()
{

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

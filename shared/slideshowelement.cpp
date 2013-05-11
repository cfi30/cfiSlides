/**
 * Copyright (C) 2013  Christian Fillion
 * This file is part of Foobar.
 *
 * Foobar is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Foobar is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "slideshowelement.h"

SlideshowElement::SlideshowElement() : BaseElement()
{
}

void SlideshowElement::bindProperties(QtTreePropertyBrowser *browser) const
{
	QtStringPropertyManager *stringManager = new QtStringPropertyManager();
	QtLineEditFactory *lineEditFactory = new QtLineEditFactory(browser);

	QtProperty *name = stringManager->addProperty(tr("Étiquette"));
	name->setWhatsThis("name");
	name->setToolTip(tr("Nom de l'élément"));
	stringManager->setValue(name, getValue("name").toString());
	stringManager->setRegExp(name, QRegExp("^([^\\s](.*[^\\s])?)$"));
	browser->addProperty(name);

	browser->setFactoryForManager(stringManager, lineEditFactory);

	connect(stringManager, SIGNAL(valueChanged(QtProperty*,QString)), this, SLOT(stringValueChanged(QtProperty*,QString)));
}

void SlideshowElement::valueChanged(QtProperty *property, QVariant value)
{
	setValue(property->whatsThis(), value);
	emit modified();
}

void SlideshowElement::stringValueChanged(QtProperty *property, QString value)
{
	valueChanged(property, value);
}

void SlideshowElement::intValueChanged(QtProperty *property, int value)
{
	valueChanged(property, value);
}

void SlideshowElement::colorValueChanged(QtProperty *property, QColor value)
{
	valueChanged(property, value);
}

void SlideshowElement::sizeValueChanged(QtProperty *property, QSize value)
{
	valueChanged(property, value);
}

void SlideshowElement::pointValueChanged(QtProperty *property, QPoint value)
{
	valueChanged(property, value);
}

void SlideshowElement::fontValueChanged(QtProperty *property, QFont value)
{
	valueChanged(property, value);
}

void SlideshowElement::boolValueChanged(QtProperty *property, bool value)
{
	valueChanged(property, value);
}

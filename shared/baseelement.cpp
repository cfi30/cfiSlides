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

#include "baseelement.h"

BaseElement::BaseElement(QObject *parent) : QObject(parent)
{
}

QVariant BaseElement::getValue(const QString &name, QVariant defaultValue) const
{
	return this->properties.value(name, defaultValue);
}

void BaseElement::setValue(const QString &name, QVariant value)
{
	this->properties[name] = value;
}

void BaseElement::setProperties(QMap<QString, QVariant> properties)
{
	QMap<QString, QVariant>::iterator iterator;
	for(iterator = properties.begin(); iterator != properties.end(); ++iterator)
		this->properties[iterator.key()] = iterator.value();
}

QMap<QString, QVariant> BaseElement::getProperties() const
{
	return this->properties;
}

void BaseElement::bindProperties(QtTreePropertyBrowser *browser) const
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

void BaseElement::valueChanged(QtProperty *property, QVariant value)
{
	setValue(property->whatsThis(), value);
	emit modified();
}

void BaseElement::stringValueChanged(QtProperty *property, QString value)
{
	valueChanged(property, value);
}

void BaseElement::intValueChanged(QtProperty *property, int value)
{
	valueChanged(property, value);
}

void BaseElement::colorValueChanged(QtProperty *property, QColor value)
{
	valueChanged(property, value);
}

void BaseElement::sizeValueChanged(QtProperty *property, QSize value)
{
	valueChanged(property, value);
}

void BaseElement::pointValueChanged(QtProperty *property, QPoint value)
{
	valueChanged(property, value);
}

void BaseElement::fontValueChanged(QtProperty *property, QFont value)
{
	valueChanged(property, value);
}

void BaseElement::boolValueChanged(QtProperty *property, bool value)
{
	valueChanged(property, value);
}

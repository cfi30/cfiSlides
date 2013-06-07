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

#include "property.h"

Property::Property(PropertyManager *manager, const QString &label, const QString &name)
{
	this->manager = manager;

	setLabel(label);
	setName(name);
}

Property::~Property()
{
	if(manager) manager->deleteLater();
}

QString Property::getLabel() const
{
	return label;
}

void Property::setLabel(const QString &label)
{
	this->label = label;
}

QString Property::getName() const
{
	return name;
}

QString Property::getToolTip() const
{
	return toolTip;
}

void Property::setToolTip(const QString &toolTip)
{
	this->toolTip = toolTip;
}

void Property::setName(const QString &name)
{
	this->name = name;
}

QVariant Property::getValue() const
{
	return value;
}

void Property::setValue(const QVariant &value)
{
	this->value = value;
}

PropertyManager *Property::getManager() const
{
	return this->manager;
}

PropertyList Property::getProperties() const
{
	return subprops;
}

void Property::addProperty(Property *prop)
{
	subprops.append(prop);
}


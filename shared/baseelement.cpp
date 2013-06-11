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

QVariant BaseElement::getValue(const QString &name, QVariant defaultValue) const
{
	return this->properties.value(name, defaultValue);
}

void BaseElement::setValue(const QString &name, QVariant value)
{
	this->properties[name] = value;
}

int BaseElement::unsetValue(const QString &name)
{
	return this->properties.remove(name);
}

void BaseElement::setValues(QVariantMap properties)
{
	QVariantMap::iterator iterator;
	for(iterator = properties.begin(); iterator != properties.end(); ++iterator)
		this->properties[iterator.key()] = iterator.value();
}

QVariantMap BaseElement::getValues() const
{
	return this->properties;
}

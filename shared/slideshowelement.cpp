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

#include "slideshowelement.h"
#include "propertymanager.h"

PropertyList SlideshowElement::getProperties() const
{
	StringPropertyManager *stringManager = new StringPropertyManager;
	connect(stringManager, &PropertyManager::modified, this, &SlideshowElement::propertyChanged);

	Property *name = new Property(stringManager, tr("Étiquette"), QStringLiteral("name"));
	name->setToolTip(tr("Nom de l'élément"));
	name->setValue(this->getValue(QStringLiteral("name")));
	stringManager->setRegExp(QStringLiteral("name"), QRegExp(QStringLiteral("^([^\\s](.*[^\\s])?)$")));
	stringManager->setRequired(QStringLiteral("name"), true);

	return PropertyList() << name;
}

void SlideshowElement::propertyChanged(const QString &name, const QVariant &value)
{
	if(this->getValue(name) != value)
	{
		this->setValue(name, value);
		emit modified();
	}
}

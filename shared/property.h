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

#ifndef PROPERTY_H
#define PROPERTY_H

#include <QObject>
#include <QList>
#include <QVariant>

#include "shared.h"

class PropertyManager;

class CFISLIDES_DLLSPEC Property
{
public:
	explicit Property(PropertyManager *manager, const QString &label, const QString &name = QString());
	~Property();
	QString getLabel() const;
	void setLabel(const QString &);
	QString getToolTip() const;
	void setToolTip(const QString &);
	QString getName() const;
	void setName(const QString &);
	QVariant getValue() const;
	void setValue(const QVariant &);
	PropertyManager *getManager() const;
	QList<Property *> getProperties() const;
	void addProperty(Property *);

protected:
	QString label;
	QString toolTip;
	QString name;
	QVariant value;
	bool required;
	PropertyManager *manager;
	QList<Property *> subprops;
};

typedef QList<Property *> PropertyList;

#endif // PROPERTY_H

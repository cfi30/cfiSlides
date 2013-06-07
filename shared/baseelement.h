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

#ifndef BASEELEMENT_H
#define BASEELEMENT_H

#include <QObject>
#include <QMap>
#include <QVariant>

#include "shared.h"

class CFISLIDES_DLLSPEC BaseElement : public QObject
{
	Q_OBJECT

public:
	BaseElement() : QObject() {}
	QVariant getValue(const QString &name, QVariant defaultValue = QVariant()) const;
	void setValue(const QString &name, QVariant value);
	int unsetValue(const QString &name);
	QMap<QString, QVariant> getValues() const;
	void setValues(QMap<QString, QVariant>);

private:
	QMap<QString, QVariant> properties;
};
#endif // BASEELEMENT_H

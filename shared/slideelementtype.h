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

#ifndef SLIDEELEMENTTYPE_H
#define SLIDEELEMENTTYPE_H

#include <QString>
#include <QIcon>
#include <QList>

#include "shared.h"

class CFISLIDES_DLLSPEC SlideElementType
{
public:
	SlideElementType() {}
	explicit SlideElementType(const int typeId, const QString &label, const QIcon &icon = QIcon());
	int getId() const;
	QString getLabel() const;
	QIcon getIcon() const;

private:
	int typeId;
	QString label;
	QIcon icon;
};

typedef QList<SlideElementType> SlideElementTypeList;

#endif // SLIDEELEMENTTYPE_H

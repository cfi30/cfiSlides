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

#ifndef HELLOELEMENT_H
#define HELLOELEMENT_H

#include <QGraphicsRectItem>
#include <QFontMetrics>

#include "slideelement.h"
#include "graphicsitem.h"

class HelloElement : public SlideElement
{
	Q_OBJECT
public:
	HelloElement() : SlideElement() {}
	virtual QGraphicsItem *render(const bool interactive);
	virtual PropertyList getProperties() const;
};

class Hello2DItem : public QGraphicsRectItem
{
	GRAPHICS_ITEM(Hello2DItem, QGraphicsRectItem)
};

Q_DECLARE_METATYPE(HelloElement)

#endif // HELLOELEMENT_H

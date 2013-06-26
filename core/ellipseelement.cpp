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

#include "ellipseelement.h"

QGraphicsItem *EllipseElement::render(const bool interactive)
{
	if(!getValue(QStringLiteral("visible")).toBool())
		return 0;

	QPen pen(penStyle());
	pen.setColor(getValue(QStringLiteral("borderColor")).value<QColor>());
	pen.setWidth(getValue(QStringLiteral("borderSize")).toInt());

	GraphicsEllipseItem *item = new GraphicsEllipseItem(interactive, this);
	item->setBrush(QBrush(getValue(QStringLiteral("color")).value<QColor>(), brushStyle()));
	item->setPen(pen);
	item->setRect(QRect(QPoint(), getValue(QStringLiteral("size")).toSize()));
	item->setPos(getValue(QStringLiteral("position")).toPoint());

	return item;
}

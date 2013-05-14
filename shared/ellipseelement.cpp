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

EllipseElement::EllipseElement() :  RectElement()
{
}

void EllipseElement::render(QGraphicsScene *scene, const bool interactive)
{
	SlideElement::render(scene, interactive);

	if(!getValue("visible").toBool())
		return;

	QPen pen(penStyle());
	pen.setColor(getValue("borderColor").value<QColor>());
	pen.setWidth(getValue("borderSize", 1).toInt());

	GraphicsEllipseItem *item = new GraphicsEllipseItem();
	item->setBrush(QBrush(getValue("color").value<QColor>(), brushStyle()));
	item->setPen(pen);
	item->setRect(QRectF(QPointF(), getValue("size").toSizeF()));
	item->setPos(getValue("position").toPoint());
	item->setElement(this);

	scene->addItem(item);
}

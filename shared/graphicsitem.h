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

#ifndef GRAPHICSITEM_H
#define GRAPHICSITEM_H

#include <QGraphicsScene>

#define GRAPHICS_ITEM(className, parentClass) \
public:\
	explicit className(const bool interactive, SlideElement *parent) : parentClass()\
	{\
		this->slideElement = parent;\
		this->setData(Qt::UserRole, parent->getIndex());\
\
		if(interactive)\
			this->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemSendsGeometryChanges);\
	}\
\
private:\
	SlideElement *slideElement;\
\
protected:\
	virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value)\
	{\
		if(change != ItemPositionChange || !scene())\
			return QGraphicsItem::itemChange(change, value);\
\
		QPointF newPos = value.toPointF();\
		QRectF rect = scene()->sceneRect();\
		if(newPos.x() < 0)\
			newPos.setX(0);\
		if(newPos.y() < 0)\
			newPos.setY(0);\
\
		if(newPos.x() + boundingRect().right() > rect.right())\
			newPos.setX(rect.right() - boundingRect().right());\
		if(newPos.y() + boundingRect().bottom() > rect.bottom())\
			newPos.setY(rect.bottom() - boundingRect().bottom());\
\
		if(this->slideElement != 0)\
			this->slideElement->movedTo(newPos.toPoint());\
		return newPos;\
	}

#endif // GRAPHICSITEM_H

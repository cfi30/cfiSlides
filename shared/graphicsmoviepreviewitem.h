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

#ifndef GRAPHICSMOVIEPREVIEWITEM_H
#define GRAPHICSMOVIEPREVIEWITEM_H

#include <QGraphicsRectItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsTextItem>
#include <QIcon>

#include "graphicsitem.h"
#include "shared.h"

class CFISLIDES_DLLSPEC GraphicsMoviePreviewItem : public QGraphicsRectItem
{
	GRAPHICS_ITEM
public:
	GraphicsMoviePreviewItem(QGraphicsItem *parent = 0);
	void setSize(const QSize &size);
	void setSource(const QString &src);

protected:
	QGraphicsPixmapItem *icon;
	QGraphicsTextItem *label;
};

#endif // GRAPHICSMOVIEPREVIEWITEM_H

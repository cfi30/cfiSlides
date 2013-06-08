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

#include "graphicsmoviepreviewitem.h"

GraphicsMoviePreviewItem::GraphicsMoviePreviewItem(QGraphicsItem *parent) : QGraphicsRectItem(parent)
{
	this->setBrush(Qt::darkGray);
	this->setPen(Qt::NoPen);

	icon = new QGraphicsPixmapItem(this);
	icon->setPixmap(QIcon::fromTheme("applications-multimedia").pixmap(QSize(128, 128)));

	label = new QGraphicsTextItem(this);
	label->setDefaultTextColor(Qt::black);
}

void GraphicsMoviePreviewItem::setSource(const QString &src)
{
	label->setPlainText(src);
}

void GraphicsMoviePreviewItem::setSize(const QSize &size)
{
	this->setRect(QRect(QPoint(), size));

	icon->setVisible(
		size.width() > icon->pixmap().size().width() &&
		size.height() > icon->pixmap().size().height()
	);

	icon->setPos(
		(size.width() - icon->pixmap().width()) / 2,
		(size.height() - icon->pixmap().height()) / 2
	);

	label->setTextWidth(size.width());
	label->setVisible(icon->isVisible());
}

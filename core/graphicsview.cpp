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

#include "graphicsview.h"

GraphicsView::GraphicsView(QGraphicsScene *scene, QWidget *parent) : QGraphicsView(scene, parent)
{
	this->scale(DEFAULT_ZOOM, DEFAULT_ZOOM);
	this->setBackgroundRole(QPalette::Dark);
	this->setDragMode(QGraphicsView::ScrollHandDrag);
	this->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
	this->zoomCount = 0;
}

void GraphicsView::moveSelectedItemsBy(const qreal dx, const qreal dy)
{
	foreach(QGraphicsItem *item, this->scene()->selectedItems())
		item->moveBy(dx, dy);
}

void GraphicsView::mousePressEvent(QMouseEvent *event)
{
	if(event->modifiers().testFlag(Qt::ShiftModifier))
		this->setDragMode(QGraphicsView::RubberBandDrag);
	else
		this->setDragMode(QGraphicsView::ScrollHandDrag);

	QGraphicsView::mousePressEvent(event);
}

void GraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
	QGraphicsView::mouseReleaseEvent(event);
	this->setDragMode(QGraphicsView::ScrollHandDrag);
}

void GraphicsView::wheelEvent(QWheelEvent *event)
{
	if(event->modifiers().testFlag(Qt::ControlModifier))
	{
		const int numSteps = event->delta() / 15 / 8;
		if (numSteps == 0)
		{
			event->ignore();
			return;
		}

		const qreal factor = pow(ZOOM_FACTOR, numSteps);
		if((zoomCount > ZOOM_LIMIT && factor > 1) || (zoomCount == 0 && factor < 1))
			return;

		if(factor > 1)
			zoomCount += 1;
		else
			zoomCount -= 1;

		scale(factor, factor);
		centerOn(mapToScene(event->pos()));
	}
	else
		QGraphicsView::wheelEvent(event);
}

void GraphicsView::keyPressEvent(QKeyEvent *event)
{
	switch(event->key())
	{
		case Qt::Key_Left:
			moveSelectedItemsBy(0 - MOVE_STEPS, 0);
			break;
		case Qt::Key_Up:
			moveSelectedItemsBy(0, 0 - MOVE_STEPS);
			break;
		case Qt::Key_Right:
			moveSelectedItemsBy(MOVE_STEPS, 0);
			break;
		case Qt::Key_Down:
			moveSelectedItemsBy(0, MOVE_STEPS);
			break;
		default:
			QGraphicsView::keyPressEvent(event);
			break;
	}
}

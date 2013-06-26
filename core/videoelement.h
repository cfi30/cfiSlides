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

#ifndef VIDEOELEMENT_H
#define VIDEOELEMENT_H

#include <QMediaPlayer>
#include <QGraphicsVideoItem>
#include <QGraphicsRectItem>
#include <QPainter>
#include <QMediaPlaylist>
#include <QApplication>

#include "slideelement.h"
#include "graphicsitem.h"

class VideoElement : public SlideElement
{
	Q_OBJECT

public:
	VideoElement();
	virtual QString previewUrl() const;
	virtual QGraphicsItem *render(const bool interactive);
	virtual PropertyList getProperties() const;

public slots:
	virtual void play();
	virtual void pause();
	virtual void stop();
	virtual void toggleMute();
	virtual void destroy();

private slots:
	void stateChanged(QMediaPlayer::State state);

protected:
	QMediaPlayer *player;
	bool playbackFinished;
};

class MoviePlaceholderItem : public QGraphicsRectItem
{
	GRAPHICS_ITEM(MoviePlaceholderItem, QGraphicsRectItem)
};

Q_DECLARE_METATYPE(VideoElement)

#endif // VIDEOELEMENT_H

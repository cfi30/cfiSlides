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

#ifndef SLIDE_H
#define SLIDE_H

#include <QObject>
#include <QWidget>
#include <QApplication>
#include <QLabel>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QColor>

#include "slideshowelement.h"
#include "slideelement.h"
#include "configuration.h"
#include "shared.h"

class CFISLIDES_DLLSPEC Slide : public SlideshowElement
{
	Q_OBJECT

public:
	Slide() : SlideshowElement() {}
	explicit Slide(const QString &name);
	~Slide();

	void render(QGraphicsScene *scene, const bool interactive);
	QList<SlideElement *> getElements() const;
	SlideElement *getElement(const int index) const;
	void addElement(SlideElement *);
	void removeElement(const int index);
	void moveElement(const int from, const int to);
	virtual PropertyList getProperties() const;

signals:
	void moved();
	void refresh();

public slots:
	void play();
	void pause();
	void stop();
	void toggleMute();
	void destroy();

protected slots:
	void elementChanged();
	void elementMoved();
	void refreshRequest();

protected:
	enum ImageStretch
	{
		Fill,
		Repeat,
		KeepRatio,
		IgnoreRatio
	};
	QList<SlideElement *> elements;
};

#endif // SLIDE_H

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

#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsItem>

#include "slideshowelement.h"
#include "slideelement.h"
#include "configuration.h"
#include "shared.h"

class Slideshow;

class CFISLIDES_DLLSPEC Slide : public SlideshowElement
{
	Q_OBJECT

public:
	explicit Slide(Slideshow *slideshow);
	~Slide();

	void render(QGraphicsScene *scene, const bool interactive) const;
	QList<SlideElement *> getElements() const;
	SlideElement *getElement(const int index) const;
	void addElement(SlideElement *);
	void removeElement(const int index);
	void moveElement(const int from, const int to);
	virtual PropertyList getProperties() const;
	Slideshow *slideshow() const;
	void setSlideshow(Slideshow *slideshow);

signals:
	void moved();
	void refresh();
	void updateProperties();

public slots:
	void play();
	void pause();
	void stop();
	void toggleMute();
	void destroy();

protected slots:
	void elementChanged();
	void elementMoved();
	void refreshRequested();
	void updatePropertiesRequested();

private:
	enum ImageStretch
	{
		Fill,
		Repeat,
		KeepRatio,
		IgnoreRatio
	};
	QList<SlideElement *> elements;
	Slideshow *parentSlideshow;
};

#endif // SLIDE_H

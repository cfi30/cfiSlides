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

#ifndef SLIDEELEMENT_H
#define SLIDEELEMENT_H

#include <QObject>

#include "slideshowelement.h"
#include "configuration.h"
#include "shared.h"

class Slide;
class Slideshow;

class CFISLIDES_DLLSPEC SlideElement : public SlideshowElement
{
	Q_OBJECT

public:
	SlideElement();
	SlideElement(const SlideElement &copy);
	virtual QString previewUrl() const;
	const char *type() const;
	virtual QGraphicsItem *render(const bool interactive) = 0;
	virtual PropertyList getProperties() const;
	int getIndex() const;
	void setIndex(const int newIndex);
	Slide *slide() const;
	void setSlide(Slide *slide);
	Slideshow *slideshow() const;
	
signals:
	void moved();
	void refresh();
	void updateProperties();
	void finished();

public slots:
	void movedTo(QPoint);
	virtual void play() {}
	virtual void pause() {}
	virtual void stop() {}
	virtual void toggleMute() {}
	virtual void destroy() {}

private:
	int elementIndex;
	Slide *parentSlide;
};

#endif // SLIDEELEMENT_H

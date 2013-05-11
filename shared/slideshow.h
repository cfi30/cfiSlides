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

#ifndef SLIDESHOW_H
#define SLIDESHOW_H

#include <QObject>
#include <QList>

#include "baseelement.h"
#include "slide.h"
#include "shared.h"

class CFISLIDES_DLLSPEC Slideshow : public BaseElement
{
	Q_OBJECT
public:
	Slideshow();
	~Slideshow();

	QList<Slide *> getSlides() const;
	void setSlides(QList<Slide *>);
	Slide *getSlide(const int index) const;
	Slide *createSlide(const QString name);
	void moveSlide(const int from, const int to);
	int indexOf(Slide *) const;
	void removeSlide(const int index);

protected:
	QList<Slide *> slides;
};

#endif // SLIDESHOW_H

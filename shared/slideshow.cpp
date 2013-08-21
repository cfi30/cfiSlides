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

#include <QDesktopWidget>

#include "slideshow.h"
#include "slide.h"

Slideshow::Slideshow() : BaseElement()
{
	setValue(QStringLiteral("size"), QDesktopWidget().screenGeometry().size());
}

Slideshow::~Slideshow()
{
	while(!slides.isEmpty())
		delete slides.takeFirst();
}

QList<Slide *> Slideshow::getSlides() const
{
	return slides;
}

Slide *Slideshow::getSlide(const int index) const
{
	return slides[index];
}

Slide *Slideshow::createSlide()
{
	Slide *slide = new Slide(this);
	slides << slide;
	return slide;
}

void Slideshow::addSlide(Slide *slide)
{
	slides << slide;
}

void Slideshow::moveSlide(const int from, const int to)
{
	slides.move(from, to);
}

int Slideshow::indexOf(Slide *s) const
{
	return slides.indexOf(s);
}

void Slideshow::removeSlide(const int index)
{
	slides[index]->deleteLater();
	slides.removeAt(index);
}

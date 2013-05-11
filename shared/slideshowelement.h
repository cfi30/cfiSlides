/**
 * Copyright (C) 2013  Christian Fillion
 * This file is part of Foobar.
 *
 * Foobar is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Foobar is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SLIDESHOWELEMENT_H
#define SLIDESHOWELEMENT_H

#include <QtProperty>
#include <QtTreePropertyBrowser>
#include <QtStringPropertyManager>
#include <QtLineEditFactory>
#include <QGraphicsScene>

#include "baseelement.h"
#include "shared.h"

class CFISLIDES_DLLSPEC SlideshowElement : public BaseElement
{
	Q_OBJECT
public:
	explicit SlideshowElement();
	virtual void bindProperties(QtTreePropertyBrowser *browser) const;
	virtual void render(QGraphicsScene *scene, const bool interactive) = 0;

signals:
	void modified();

protected slots:
	void valueChanged(QtProperty *, QVariant);
	void stringValueChanged(QtProperty *, QString);
	void intValueChanged(QtProperty *, int);
	void colorValueChanged(QtProperty *, QColor);
	void sizeValueChanged(QtProperty *, QSize);
	void pointValueChanged(QtProperty *, QPoint);
	void fontValueChanged(QtProperty *, QFont);
	void boolValueChanged(QtProperty *, bool);
};

#endif // SLIDESHOWELEMENT_H

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

#ifndef LINEELEMENT_H
#define LINEELEMENT_H

#include <QGraphicsScene>

#include "slideelement.h"
#include "graphicslineitem.h"
#include "shared.h"

class CFISLIDES_DLLSPEC LineElement : public SlideElement
{
	Q_OBJECT

public:
	LineElement();
	virtual void render(QGraphicsScene *scene, const bool interactive);
	virtual PropertyList getProperties() const;
};

Q_DECLARE_METATYPE(LineElement)

#endif // LINEELEMENT_H

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

#include "lineelement.h"

LineElement::LineElement() : SlideElement()
{
	setValue("size", 4);
	setValue("stop", QPoint(300, 0));
}

void LineElement::render(QGraphicsScene *scene, const bool interactive)
{
	SlideElement::render(scene, interactive);

	if(!getValue("visible").toBool())
		return;

	Qt::PenStyle penStyle;
	switch(getValue("style").toInt())
	{
		case 1:
			penStyle = Qt::DashLine;
			break;
		case 2:
			penStyle = Qt::DotLine;
			break;
		case 3:
			penStyle = Qt::DashDotLine;
			break;
		case 4:
			penStyle = Qt::DashDotDotLine;
			break;
		default:
			penStyle = Qt::SolidLine;
			break;
	}

	QPen pen(penStyle);
	pen.setColor(getValue("color").value<QColor>());
	pen.setWidth(getValue("size").toInt());

	GraphicsLineItem *item = new GraphicsLineItem(this);
	item->setPen(pen);
	item->setPos(getValue("position").toPoint());
	item->setLine(QLine(QPoint(0, getValue("start").toInt()), getValue("stop").toPoint()));

	scene->addItem(item);
}

PropertyList LineElement::getProperties() const
{
	BoolPropertyManager *boolManager = new BoolPropertyManager;
	connect(boolManager, &PropertyManager::modified, this, &LineElement::propertyChanged);

	PointPropertyManager *pointManager = new PointPropertyManager;
	connect(pointManager, &PropertyManager::modified, this, &LineElement::propertyChanged);

	IntPropertyManager *intManager = new IntPropertyManager;
	connect(intManager, &PropertyManager::modified, this, &LineElement::propertyChanged);

	ColorPropertyManager *colorManager = new ColorPropertyManager;
	connect(colorManager, &PropertyManager::modified, this, &LineElement::propertyChanged);

	EnumPropertyManager *enumManager = new EnumPropertyManager;
	connect(enumManager, &PropertyManager::modified, this, &LineElement::propertyChanged);

	Property *visible = new Property(boolManager, tr("Visible"), "visible");
	visible->setToolTip(tr("Visibilité de l'élément"));
	visible->setValue(this->getValue("visible"));

	Property *geometry = new Property(0, tr("Géométrie"));

	Property *position = new Property(pointManager, tr("Position"), "position");
	position->setToolTip(tr("Position de l'élément"));
	position->setValue(this->getValue("position"));
	geometry->addProperty(position);

	Property *start = new Property(intManager, tr("Départ"), "start");
	start->setToolTip(tr("Point de départ vertical de la ligne"));
	start->setValue(this->getValue("start"));
	intManager->setSuffix("start", tr(" px"));
	geometry->addProperty(start);

	Property *stop = new Property(pointManager, tr("Arrivée"), "stop");
	stop->setToolTip(tr("Point d'arrivée de la ligne"));
	stop->setValue(this->getValue("stop"));
	geometry->addProperty(stop);

	Property *group = new Property(0, tr("Ligne"));

	Property *size = new Property(intManager, tr("Épaisseur"), "size");
	size->setToolTip(tr("Épaisseur de la bordure"));
	size->setValue(this->getValue("size"));
	intManager->setMinimum("size", 1);
	intManager->setMaximum("size", MAXIMUM_THICKNESS);
	intManager->setSuffix("size", tr(" px"));
	group->addProperty(size);

	Property *color = new Property(colorManager, tr("Couleur"), "color");
	color->setToolTip(tr("Couleur de la ligne"));
	color->setValue(this->getValue("color"));
	group->addProperty(color);

	Property *style = new Property(enumManager, tr("Style"), "style");
	style->setToolTip(tr("Style de la ligne"));
	style->setValue(this->getValue("style"));
	enumManager->setEnumNames("style", QStringList() << tr("Solide") << tr("Pointillés") << tr("Points") << tr("Point-tiret") << tr("Point-point-tiret"));
	group->addProperty(style);

	return PropertyList()
		<< SlideshowElement::getProperties()
		<< visible
		<< geometry
		<< group;
}

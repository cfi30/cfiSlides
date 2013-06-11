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

#include "helloelement.h"

void HelloElement::render(QGraphicsScene *scene, const bool interactive)
{
	SlideElement::render(scene, interactive);

	if(!getValue("visible").toBool())
		return;

	QBrush brush;
	brush.setStyle(Qt::DiagCrossPattern);
	brush.setColor(Qt::black);

	Hello2DItem *item = new Hello2DItem(this);
	item->setPen(QPen(Qt::NoPen));
	item->setBrush(brush);
	item->setPos(getValue("position").toPoint());

	scene->addItem(item);

	QFont font(QString(), 42, QFont::Bold);
	QGraphicsTextItem *subitem = new QGraphicsTextItem(item);
	subitem->setFont(font);
	subitem->setHtml(QString("<span style=\"color: green\">HELLO <i style=\"color: purple\">%1</i> !!</span>").arg(getValue("hello").toString()));

	item->setRect(QRect(QPoint(), QFontMetrics(font).boundingRect(subitem->toPlainText()).size() + QSize(50, 20)));
}

PropertyList HelloElement::getProperties() const
{
	StringPropertyManager *stringManager = new StringPropertyManager;
	connect(stringManager, &PropertyManager::modified, this, &HelloElement::propertyChanged);

	Property *group = new Property(0, "Hello World");

	Property *text = new Property(stringManager, "Hello", "hello");
	text->setToolTip("Say hello to who?");
	text->setValue(this->getValue("hello"));
	stringManager->setPlaceholder("hello", "(nobody)");
	stringManager->setRequired("hello", true);
	group->addProperty(text);

	return PropertyList()
		<< SlideElement::getProperties()
		<< group;
}

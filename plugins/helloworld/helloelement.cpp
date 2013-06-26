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

QGraphicsItem *HelloElement::render(const bool interactive)
{
	if(!getValue(QStringLiteral("visible")).toBool())
		return 0;

	QBrush brush;
	brush.setStyle(Qt::DiagCrossPattern);
	brush.setColor(Qt::black);

	Hello2DItem *item = new Hello2DItem(interactive, this);
	item->setPen(QPen(Qt::NoPen));
	item->setBrush(brush);
	item->setPos(getValue(QStringLiteral("position")).toPoint());

	QFont font(QString(), 42, QFont::Bold);
	QGraphicsTextItem *subitem = new QGraphicsTextItem(item);
	subitem->setFont(font);
	subitem->setHtml(
		QString("<span style=\"color: green\">HELLO <i style=\"color: purple\">%1</i> !!</span>")
		.arg(getValue(QStringLiteral("hello")).toString().toHtmlEscaped())
	);
	item->setRect(QRect(QPoint(), QFontMetrics(font).boundingRect(subitem->toPlainText()).size() + QSize(50, 20)));

	return item;
}

PropertyList HelloElement::getProperties() const
{
	StringPropertyManager *stringManager = new StringPropertyManager;
	connect(stringManager, SIGNAL(modified(QString, QVariant)), this, SLOT(propertyChanged(QString, QVariant)));

	Property *group = new Property(0, "Hello World");

	Property *text = new Property(stringManager, "Hello", QStringLiteral("hello"));
	text->setToolTip("Say hello to who?");
	text->setValue(this->getValue(QStringLiteral("hello")));
	stringManager->setPlaceholder(QStringLiteral("hello"), "(nobody)");
	stringManager->setRequired(QStringLiteral("hello"), true);
	group->addProperty(text);

	return PropertyList()
		<< SlideElement::getProperties()
		<< group;
}

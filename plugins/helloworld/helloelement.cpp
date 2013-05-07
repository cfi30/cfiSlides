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

HelloElement::HelloElement() : SlideElement()
{
}

void HelloElement::render(QGraphicsScene *scene, const bool interactive)
{
	SlideElement::render(scene, interactive);

	if(!getValue("visible").toBool())
		return;

	QBrush brush;
	brush.setStyle(Qt::DiagCrossPattern);
	brush.setColor(Qt::black);

	Hello2DItem *item = new Hello2DItem();
	item->setPen(QPen(Qt::NoPen));
	item->setBrush(brush);
	item->setPos(getValue("position").toPoint());
	item->setData(Qt::UserRole, getIndex());
	if(interactive)
	{
		item->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemSendsGeometryChanges);
		item->setElement(this);
	}
	scene->addItem(item);

	QFont font(QString(), 42, QFont::Bold);
	QGraphicsTextItem *subitem = new QGraphicsTextItem(item);
	subitem->setFont(font);
	subitem->setHtml(QString("<span style=\"color: green\">HELLO <i style=\"color: purple\">%1</i> !!</span>").arg(getValue("hello").toString()));

	item->setRect(QRectF(QPointF(), QFontMetrics(font).boundingRect(subitem->toPlainText()).size() + QSize(50, 20)));
}

void HelloElement::bindProperties(QtTreePropertyBrowser *browser) const
{
	SlideElement::bindProperties(browser);

	QtGroupPropertyManager *groupManager = new QtGroupPropertyManager;
	QtStringPropertyManager *stringManager = new QtStringPropertyManager;
	QtLineEditFactory *lineEditFactory = new QtLineEditFactory(browser);

	QtProperty *group = groupManager->addProperty("Hello World");
	group->setModified(true); // groups should be displayed in bold

	QtProperty *text = stringManager->addProperty("Hello");
	text->setWhatsThis("hello");
	text->setToolTip("Say hello to who?");
	stringManager->setValue(text, getValue("hello").toString());
	group->addSubProperty(text);

	browser->addProperty(group);

	browser->setFactoryForManager(stringManager, lineEditFactory);

	connect(stringManager, SIGNAL(valueChanged(QtProperty*,QString)), this, SLOT(stringValueChanged(QtProperty*,QString)));
}

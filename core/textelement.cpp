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

#include "textelement.h"

TextElement::TextElement() : SlideElement()
{
	QFont font;
	font.setPointSize(20);

	setValue("text", "Lorem ipsum dolor sit amet");
	setValue("font", font);
	setValue("width", 400);
}

void TextElement::render(QGraphicsScene *scene, const bool interactive)
{
	SlideElement::render(scene, interactive);

	if(!getValue("visible").toBool())
		return;

	GraphicsTextItem *item = new GraphicsTextItem(this);
	item->setPlainText(getValue("text").toString());
	item->setFont(getValue("font").value<QFont>());
	item->setDefaultTextColor(getValue("color").value<QColor>());
	item->setTextWidth(getValue("width").toInt());
	item->setPos(getValue("position").toPoint());

	connect(item->document(), SIGNAL(contentsChanged()), this, SLOT(textChanged()));
	scene->addItem(item);
}

PropertyList TextElement::getProperties() const
{
	BoolPropertyManager *boolManager = new BoolPropertyManager;
	connect(boolManager, SIGNAL(modified(QString, QVariant)), this, SLOT(propertyChanged(QString, QVariant)));

	PointPropertyManager *pointManager = new PointPropertyManager;
	connect(pointManager, SIGNAL(modified(QString, QVariant)), this, SLOT(propertyChanged(QString, QVariant)));

	IntPropertyManager *intManager = new IntPropertyManager;
	connect(intManager, SIGNAL(modified(QString, QVariant)), this, SLOT(propertyChanged(QString, QVariant)));

	TextPropertyManager *textManager = new TextPropertyManager;
	connect(textManager, SIGNAL(modified(QString, QVariant)), this, SLOT(propertyChanged(QString, QVariant)));

	ColorPropertyManager *colorManager = new ColorPropertyManager;
	connect(colorManager, SIGNAL(modified(QString, QVariant)), this, SLOT(propertyChanged(QString, QVariant)));

	FontPropertyManager *fontManager = new FontPropertyManager;
	connect(fontManager, SIGNAL(modified(QString, QVariant)), this, SLOT(propertyChanged(QString, QVariant)));

	Property *visible = new Property(boolManager, tr("Visible"), "visible");
	visible->setToolTip(tr("Visibilité de l'élément"));
	visible->setValue(this->getValue("visible"));

	Property *geometry = new Property(0, tr("Géométrie"));

	Property *position = new Property(pointManager, tr("Position"), "position");
	position->setToolTip(tr("Position de l'élément"));
	position->setValue(this->getValue("position"));
	geometry->addProperty(position);

	Property *width = new Property(intManager, tr("Largeur"), "width");
	width->setToolTip(tr("Taille de l'élément"));
	width->setValue(this->getValue("width"));
	intManager->setMinimum("width", 50);
	if(this->scene != 0)
		intManager->setMaximum("width", scene->sceneRect().width());
	intManager->setSuffix("width", tr(" px"));
	geometry->addProperty(width);

	Property *text = new Property(0, tr("Texte"));

	Property *body = new Property(textManager, tr("Contenu"), "text");
	body->setToolTip(tr("Corps du texte"));
	body->setValue(this->getValue("text"));
	textManager->setRequired("text", true);
	text->addProperty(body);

	Property *color = new Property(colorManager, tr("Couleur"), "color");
	color->setToolTip(tr("Couleur du texte"));
	color->setValue(this->getValue("color"));
	text->addProperty(color);

	Property *font = new Property(fontManager, tr("Police"), "font");
	font->setToolTip(tr("Police du texte"));
	font->setValue(this->getValue("font"));
	text->addProperty(font);

	return PropertyList()
		<< SlideshowElement::getProperties()
		<< visible
		<< geometry
		<< text;
}

void TextElement::textChanged()
{
	QTextDocument *document = qobject_cast<QTextDocument *>(sender());
	setValue("text", document->toPlainText());
	emit moved();
}

void GraphicsTextItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
	QGraphicsTextItem::mouseDoubleClickEvent(event);
	if(!this->flags().testFlag(QGraphicsItem::ItemIsSelectable))
		return;

	setTextInteractionFlags(Qt::TextEditorInteraction);
}

void GraphicsTextItem::focusOutEvent(QFocusEvent *event)
{
	QGraphicsTextItem::focusOutEvent(event);

	if(!this->flags().testFlag(QGraphicsItem::ItemIsSelectable))
		return;

	setTextInteractionFlags(Qt::NoTextInteraction);
	QTextCursor c(document());
	c.clearSelection();
	setTextCursor(c);
}

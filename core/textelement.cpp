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

	setValue(QStringLiteral("text"), "Lorem ipsum dolor sit amet");
	setValue(QStringLiteral("font"), font);
	setValue(QStringLiteral("width"), 400);
}

void TextElement::render(QGraphicsScene *scene, const bool interactive)
{
	SlideElement::render(scene, interactive);

	if(!getValue(QStringLiteral("visible")).toBool())
		return;

	GraphicsTextItem *item = new GraphicsTextItem(this);
	item->setPlainText(getValue(QStringLiteral("text")).toString());
	item->setFont(getValue(QStringLiteral("font")).value<QFont>());
	item->setDefaultTextColor(getValue(QStringLiteral("color")).value<QColor>());
	item->setTextWidth(getValue(QStringLiteral("width")).toInt());
	item->setPos(getValue(QStringLiteral("position")).toPoint());

	connect(item->document(), &QTextDocument::contentsChanged, this, &TextElement::textChanged);
	scene->addItem(item);
}

PropertyList TextElement::getProperties() const
{
	BoolPropertyManager *boolManager = new BoolPropertyManager;
	connect(boolManager, &PropertyManager::modified, this, &TextElement::propertyChanged);

	PointPropertyManager *pointManager = new PointPropertyManager;
	connect(pointManager, &PropertyManager::modified, this, &TextElement::propertyChanged);

	IntPropertyManager *intManager = new IntPropertyManager;
	connect(intManager, &PropertyManager::modified, this, &TextElement::propertyChanged);

	TextPropertyManager *textManager = new TextPropertyManager;
	connect(textManager, &PropertyManager::modified, this, &TextElement::propertyChanged);

	ColorPropertyManager *colorManager = new ColorPropertyManager;
	connect(colorManager, &PropertyManager::modified, this, &TextElement::propertyChanged);

	FontPropertyManager *fontManager = new FontPropertyManager;
	connect(fontManager, &PropertyManager::modified, this, &TextElement::propertyChanged);

	Property *visible = new Property(boolManager, tr("Visible"), QStringLiteral("visible"));
	visible->setToolTip(tr("Visibilité de l'élément"));
	visible->setValue(this->getValue(QStringLiteral("visible")));

	Property *geometry = new Property(0, tr("Géométrie"));

	Property *position = new Property(pointManager, tr("Position"), QStringLiteral("position"));
	position->setToolTip(tr("Position de l'élément"));
	position->setValue(this->getValue(QStringLiteral("position")));
	geometry->addProperty(position);

	Property *width = new Property(intManager, tr("Largeur"), QStringLiteral("width"));
	width->setToolTip(tr("Taille de l'élément"));
	width->setValue(this->getValue(QStringLiteral("width")));
	intManager->setMinimum(QStringLiteral("width"), 50);
	if(this->scene != 0)
		intManager->setMaximum(QStringLiteral("width"), scene->sceneRect().width());
	intManager->setSuffix(QStringLiteral("width"), tr(" px"));
	geometry->addProperty(width);

	Property *text = new Property(0, tr("Texte"));

	Property *body = new Property(textManager, tr("Contenu"), QStringLiteral("text"));
	body->setToolTip(tr("Corps du texte"));
	body->setValue(this->getValue(QStringLiteral("text")));
	textManager->setRequired(QStringLiteral("text"), true);
	text->addProperty(body);

	Property *color = new Property(colorManager, tr("Couleur"), QStringLiteral("color"));
	color->setToolTip(tr("Couleur du texte"));
	color->setValue(this->getValue(QStringLiteral("color")));
	text->addProperty(color);

	Property *font = new Property(fontManager, tr("Police"), QStringLiteral("font"));
	font->setToolTip(tr("Police du texte"));
	font->setValue(this->getValue(QStringLiteral("font")));
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
	setValue(QStringLiteral("text"), document->toPlainText());
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

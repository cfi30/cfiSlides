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
	setValue("text", "Lorem ipsum dolor sit amet");
	setValue("font", QFont(QString(), 20));
	setValue("width", 400);
}

void TextElement::render(QGraphicsScene *scene, const bool interactive)
{
	SlideElement::render(scene, interactive);

	if(!getValue("visible").toBool())
		return;

	GraphicsTextItem *item = new GraphicsTextItem();
	item->setPlainText(getValue("text").toString());
	item->setFont(getValue("font").value<QFont>());
	item->setDefaultTextColor(getValue("color").value<QColor>());
	item->setTextWidth(getValue("width").toInt());
	item->setPos(getValue("position").toPoint());
	item->setElement(this);

	connect(item->document(), SIGNAL(contentsChanged()), this, SLOT(textChanged()));
	scene->addItem(item);
}

void TextElement::bindProperties(QtTreePropertyBrowser *browser) const
{
	SlideshowElement::bindProperties(browser);

	QtBoolPropertyManager *boolManager = new QtBoolPropertyManager();
	QtCheckBoxFactory *checkBoxFactory = new QtCheckBoxFactory(browser);
	QtGroupPropertyManager *groupManager = new QtGroupPropertyManager();
	QtIntPropertyManager *intManager = new QtIntPropertyManager();
	QtPointPropertyManager *pointManager = new QtPointPropertyManager();
	QtColorPropertyManager *colorManager = new QtColorPropertyManager();
	QtColorEditorFactory *colorEditorFactory = new QtColorEditorFactory(browser);
	QtSpinBoxFactory *spinboxFactory = new QtSpinBoxFactory(browser);
	QtFontPropertyManager *fontManager = new QtFontPropertyManager();
	QtFontEditorFactory *fontEditorFactory = new QtFontEditorFactory(browser);
	QtEnumEditorFactory *enumEditorFactory = new QtEnumEditorFactory(browser);
	QtStringPropertyManager *stringManager = new QtStringPropertyManager();
	TextEditFactory *textEditFactory = new TextEditFactory(browser);

	QtProperty *visible = boolManager->addProperty(tr("Visible"));
	visible->setWhatsThis("visible");
	visible->setToolTip(tr("Visibilité de l'élément"));
	boolManager->setValue(visible, getValue("visible").toBool());
	browser->addProperty(visible);

	QtProperty *geometry = groupManager->addProperty(tr("Géométrie"));
	geometry->setModified(true);

	QtProperty *width = intManager->addProperty(tr("Largeur"));
	width->setWhatsThis("width");
	width->setToolTip(tr("Taille de l'élément"));
	intManager->setValue(width, getValue("width").toInt());
	intManager->setMinimum(width, 50);
	if(this->scene != 0)
		intManager->setMaximum(width, scene->sceneRect().width());
	geometry->addSubProperty(width);

	QtProperty *position = pointManager->addProperty(tr("Position"));
	position->setWhatsThis("position");
	position->setToolTip(tr("Position de l'élément"));
	pointManager->setValue(position, getValue("position").toPoint());
	geometry->addSubProperty(position);

	browser->addProperty(geometry);

	QtProperty *text = groupManager->addProperty(tr("Texte"));
	//text->setModified(true);

	QtProperty *body = stringManager->addProperty(tr("Modifier"));
	body->setWhatsThis("text");
	body->setToolTip(tr("Corps du texte"));
	stringManager->setValue(body, getValue("text").toString());
	text->addSubProperty(body);

	QtProperty *color = colorManager->addProperty(tr("Couleur"));
	color->setWhatsThis("color");
	color->setToolTip(tr("Couleur du texte"));
	colorManager->setValue(color, getValue("color").value<QColor>());
	text->addSubProperty(color);

	QtProperty *font = fontManager->addProperty(tr("Police"));
	font->setWhatsThis("font");
	font->setToolTip(tr("Police du texte"));
	fontManager->setValue(font, getValue("font").value<QFont>());
	text->addSubProperty(font);

	browser->addProperty(text);

	browser->setFactoryForManager(boolManager, checkBoxFactory);
	browser->setFactoryForManager(intManager, spinboxFactory);
	browser->setFactoryForManager(pointManager->subIntPropertyManager(), spinboxFactory);
	browser->setFactoryForManager(colorManager, colorEditorFactory);
	browser->setFactoryForManager(colorManager->subIntPropertyManager(), spinboxFactory);
	browser->setFactoryForManager(fontManager, fontEditorFactory);
	browser->setFactoryForManager(fontManager->subBoolPropertyManager(), checkBoxFactory);
	browser->setFactoryForManager(fontManager->subEnumPropertyManager(), enumEditorFactory);
	browser->setFactoryForManager(fontManager->subIntPropertyManager(), spinboxFactory);
	browser->setFactoryForManager(stringManager, textEditFactory);

	connect(boolManager, SIGNAL(valueChanged(QtProperty*,bool)), this, SLOT(boolValueChanged(QtProperty*,bool)));
	connect(intManager, SIGNAL(valueChanged(QtProperty*,int)), this, SLOT(intValueChanged(QtProperty*,int)));
	connect(pointManager, SIGNAL(valueChanged(QtProperty*,QPoint)), this, SLOT(pointValueChanged(QtProperty*,QPoint)));
	connect(colorManager, SIGNAL(valueChanged(QtProperty*,QColor)), this, SLOT(colorValueChanged(QtProperty*,QColor)));
	connect(fontManager, SIGNAL(valueChanged(QtProperty*,QFont)), this, SLOT(fontValueChanged(QtProperty*,QFont)));
	connect(stringManager, SIGNAL(valueChanged(QtProperty*,QString)), this, SLOT(stringValueChanged(QtProperty*,QString)));
}

void TextElement::textChanged()
{
	QTextDocument *document = qobject_cast<QTextDocument *>(sender());
	setValue("text", document->toPlainText());
	emit moved();
}

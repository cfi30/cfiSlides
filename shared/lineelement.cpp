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

	const QPoint pos = getValue("position").toPoint();
	GraphicsLineItem *item = new GraphicsLineItem();
	item->setPen(pen);
	item->setPos(pos);
	item->setLine(QLineF(QPoint(0, getValue("start").toInt()), getValue("stop").toPoint()));
	item->setData(Qt::UserRole, getIndex());
	if(interactive)
	{
		item->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemSendsGeometryChanges);
		item->setElement(this);
	}
	scene->addItem(item);
}

void LineElement::bindProperties(QtTreePropertyBrowser *browser) const
{
	SlideshowElement::bindProperties(browser);

	QtBoolPropertyManager *boolManager = new QtBoolPropertyManager();
	QtCheckBoxFactory *checkBoxFactory = new QtCheckBoxFactory(browser);
	QtGroupPropertyManager *groupManager = new QtGroupPropertyManager();
	QtPointPropertyManager *pointManager = new QtPointPropertyManager();
	QtSpinBoxFactory *spinboxFactory = new QtSpinBoxFactory();
	QtColorPropertyManager *colorManager = new QtColorPropertyManager();
	QtColorEditorFactory *colorEditorFactory = new QtColorEditorFactory(browser);
	QtIntPropertyManager *intManager = new QtIntPropertyManager();
	QtEnumPropertyManager *enumManager = new QtEnumPropertyManager();
	QtEnumEditorFactory *enumEditorFactory = new QtEnumEditorFactory(browser);

	QtProperty *visible = boolManager->addProperty(tr("Visible"));
	visible->setWhatsThis("visible");
	visible->setToolTip(tr("Visibilité de l'élément"));
	boolManager->setValue(visible, getValue("visible").toBool());
	browser->addProperty(visible);

	QtProperty *geometry = groupManager->addProperty(tr("Géométrie"));
	geometry->setModified(true);

	QtProperty *position = pointManager->addProperty(tr("Position"));
	position->setWhatsThis("position");
	position->setToolTip(tr("Position de l'élément"));
	pointManager->setValue(position, getValue("position").toPoint());
	geometry->addSubProperty(position);

	QtProperty *start = intManager->addProperty(tr("Départ"));
	start->setWhatsThis("start");
	start->setToolTip(tr("Point de départ vertical de la ligne"));
	intManager->setValue(start, getValue("start").toInt());
	intManager->setMinimum(start, 0);
	geometry->addSubProperty(start);

	QtProperty *stop = pointManager->addProperty(tr("Arrivée"));
	stop->setWhatsThis("stop");
	stop->setToolTip(tr("Point d'arrivée de la ligne"));
	pointManager->setValue(stop, getValue("stop").toPoint());
	geometry->addSubProperty(stop);

	QtProperty *size = intManager->addProperty(tr("Épaisseur"));
	size->setWhatsThis("size");
	size->setToolTip(tr("Épaisseur de la bordure"));
	intManager->setValue(size, getValue("size").toInt());
	intManager->setMinimum(size, 1);
	intManager->setMaximum(size, 50);
	geometry->addSubProperty(size);

	browser->addProperty(geometry);

	QtProperty *group = groupManager->addProperty(tr("Ligne"));
	group->setModified(true);

	QtProperty *color = colorManager->addProperty(tr("Couleur"));
	color->setWhatsThis("color");
	color->setToolTip(tr("Couleur de la ligne"));
	colorManager->setValue(color, getValue("color").value<QColor>());
	group->addSubProperty(color);

	QtProperty *style = enumManager->addProperty(tr("Style"));
	style->setWhatsThis("style");
	style->setToolTip(tr("Style de la ligne"));
	enumManager->setEnumNames(style, QStringList() << tr("Solide") << tr("Pointillés") << tr("Points") << tr("Point-tiret") << tr("Point-point-tiret"));
	enumManager->setValue(style, getValue("style").toInt());
	group->addSubProperty(style);

	browser->addProperty(group);

	foreach(QtProperty *prop, pointManager->subIntPropertyManager()->properties())
		pointManager->subIntPropertyManager()->setMinimum(prop, 0);

	browser->setFactoryForManager(boolManager, checkBoxFactory);
	browser->setFactoryForManager(pointManager->subIntPropertyManager(), spinboxFactory);
	browser->setFactoryForManager(pointManager->subIntPropertyManager(), spinboxFactory);
	browser->setFactoryForManager(colorManager, colorEditorFactory);
	browser->setFactoryForManager(colorManager->subIntPropertyManager(), spinboxFactory);
	browser->setFactoryForManager(enumManager, enumEditorFactory);
	browser->setFactoryForManager(intManager, spinboxFactory);

	connect(boolManager, SIGNAL(valueChanged(QtProperty*,bool)), this, SLOT(boolValueChanged(QtProperty*,bool)));
	connect(pointManager, SIGNAL(valueChanged(QtProperty*,QPoint)), this, SLOT(pointValueChanged(QtProperty*,QPoint)));
	connect(pointManager, SIGNAL(valueChanged(QtProperty*,QPoint)), this, SLOT(pointValueChanged(QtProperty*,QPoint)));
	connect(colorManager, SIGNAL(valueChanged(QtProperty*,QColor)), this, SLOT(colorValueChanged(QtProperty*,QColor)));
	connect(enumManager, SIGNAL(valueChanged(QtProperty*,int)), this, SLOT(intValueChanged(QtProperty*,int)));
	connect(intManager, SIGNAL(valueChanged(QtProperty*,int)), this, SLOT(intValueChanged(QtProperty*,int)));
}

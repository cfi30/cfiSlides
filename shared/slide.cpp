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

#include "slide.h"

Slide::Slide() : SlideshowElement()
{
}

Slide::Slide(const QString name) : SlideshowElement()
{
	this->setValue("name", name);
	this->setValue("backgroundColor", Qt::white);
}

Slide::~Slide()
{
	while(!this->elements.isEmpty())
		delete this->elements.takeFirst();
}

void Slide::render(QGraphicsScene *scene, const bool interactive)
{
	QBrush background;
	QPixmap backgroundPixmap(this->getValue("backgroundImage").toString());
	if(backgroundPixmap.isNull())
	{
		background.setColor(getValue("backgroundColor").value<QColor>());
		background.setStyle(Qt::SolidPattern);
	}
	else
	{
		switch(getValue("backgroundImageStretch").toInt())
		{
			case Slide::Repeat:
				break;
			case Slide::IgnoreRatio:
				backgroundPixmap = backgroundPixmap.scaled(scene->sceneRect().width(), scene->sceneRect().height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
				break;
			case Slide::Fill:
				backgroundPixmap = backgroundPixmap.scaled(scene->sceneRect().width(), scene->sceneRect().height(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
				break;
			case Slide::KeepRatio:
				backgroundPixmap = backgroundPixmap.scaled(scene->sceneRect().width(), scene->sceneRect().height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
				break;
		}
		background.setTexture(backgroundPixmap);
	}

	scene->addRect(scene->sceneRect(), QPen(Qt::NoPen), background);

	foreach(SlideElement *element, this->elements)
		element->render(scene, interactive);
}

QList<SlideElement *> Slide::getElements() const
{
	return this->elements;
}

SlideElement *Slide::getElement(const int index) const
{
	return this->elements[index];
}

void Slide::addElement(SlideElement *element)
{
	connect(element, SIGNAL(modified()), this, SLOT(elementChanged()));
	connect(element, SIGNAL(moved()), this, SLOT(elementMoved()));
	connect(element, SIGNAL(refresh()), this, SLOT(refreshQuery()));
	element->setIndex(this->elements.count());
	this->elements.append(element);
}

void Slide::removeElement(const int index)
{
	delete getElement(index);
	this->elements.removeAt(index);

	int elementsCount = this->elements.count();
	for(int i = 0; i < elementsCount; i++)
		this->elements[i]->setIndex(i);
}

void Slide::moveElement(const int from, const int to)
{
	this->elements.move(from, to);

	int elementsCount = this->elements.count();
	for(int i = 0; i < elementsCount; i++)
		this->elements[i]->setIndex(i);
}

void Slide::bindProperties(QtTreePropertyBrowser *browser) const
{
	SlideshowElement::bindProperties(browser);

	QtGroupPropertyManager *groupManager = new QtGroupPropertyManager();
	QtColorPropertyManager *colorManager = new QtColorPropertyManager();
	QtColorEditorFactory *colorEditorFactory = new QtColorEditorFactory(browser);
	QtSpinBoxFactory *spinboxFactory = new QtSpinBoxFactory(browser);
	FilePathManager *filePathManager = new FilePathManager();
	FileEditFactory *fileEditFactory = new FileEditFactory(browser);
	QtEnumPropertyManager *enumManager = new QtEnumPropertyManager();
	QtEnumEditorFactory *enumEditorFactory = new QtEnumEditorFactory(browser);

	QtProperty *background = groupManager->addProperty(tr("Arrière-plan"));
	background->setModified(true);

	QtProperty *backgroundColor = colorManager->addProperty(tr("Couleur"));
	backgroundColor->setWhatsThis("backgroundColor");
	backgroundColor->setToolTip(tr("Couleur de fond"));
	colorManager->setValue(backgroundColor, getValue("backgroundColor").value<QColor>());
	background->addSubProperty(backgroundColor);

	QtProperty *backgroundImage = filePathManager->addProperty(tr("Image"));
	backgroundImage->setWhatsThis("backgroundImage");
	backgroundImage->setToolTip(tr("Image de fond"));
	filePathManager->setValue(backgroundImage, getValue("backgroundImage").toString());
	filePathManager->setFilter(backgroundImage, IMAGE_FILTER);
	background->addSubProperty(backgroundImage);

	QtProperty *backgroundImageStretch = enumManager->addProperty(tr("Mise à l'échelle"));
	backgroundImageStretch->setWhatsThis("backgroundImageStretch");
	backgroundImageStretch->setToolTip(tr("Mode de mise à l'échelle de l'image"));
	enumManager->setEnumNames(backgroundImageStretch, QStringList() << tr("Remplir & Conserver") << tr("Répéter") << tr("Conserver"));
	enumManager->setValue(backgroundImageStretch, getValue("backgroundImageStretch").toInt());
	background->addSubProperty(backgroundImageStretch);

	browser->addProperty(background);

	browser->setFactoryForManager(colorManager, colorEditorFactory);
	browser->setFactoryForManager(colorManager->subIntPropertyManager(), spinboxFactory);
	browser->setFactoryForManager(filePathManager, fileEditFactory);
	browser->setFactoryForManager(enumManager, enumEditorFactory);

	connect(colorManager, SIGNAL(valueChanged(QtProperty*,QColor)), this, SLOT(colorValueChanged(QtProperty*,QColor)));
	connect(filePathManager, SIGNAL(valueChanged(QtProperty*,QString)), this, SLOT(stringValueChanged(QtProperty*,QString)));
	connect(enumManager, SIGNAL(valueChanged(QtProperty*,int)), this, SLOT(intValueChanged(QtProperty*,int)));
}

void Slide::elementChanged()
{
	emit modified();
}

void Slide::elementMoved()
{
	emit moved();
}

void Slide::refreshQuery()
{
	emit refresh();
}

void Slide::play()
{
	foreach(SlideElement *element, elements)
	{
		if(element->getValue("visible").toBool())
			element->play();
	}
}

void Slide::pause()
{
	foreach(SlideElement *element, elements)
	{
		if(element->getValue("visible").toBool())
			element->pause();
	}
}

void Slide::stop()
{
	foreach(SlideElement *element, elements)
	{
		if(element->getValue("visible").toBool())
			element->stop();
	}
}

void Slide::toggleMute()
{
	foreach(SlideElement *element, elements)
	{
		if(element->getValue("visible").toBool())
			element->toggleMute();
	}
}

void Slide::destroy()
{
	foreach(SlideElement *element, elements)
	{
		if(element->getValue("visible").toBool())
			element->destroy();
	}
}

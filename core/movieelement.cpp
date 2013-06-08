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

#include "movieelement.h"

MovieElement::MovieElement() : SlideElement()
{
	playbackFinished = false;
	setValue("size", QSize(600, 400));
	setValue("volume", 100);
}

QString MovieElement::previewUrl() const
{
	return getValue("src").toString();
}

void MovieElement::render(QGraphicsScene *scene, const bool interactive)
{
	SlideElement::render(scene, interactive);

	if(!getValue("visible").toBool())
		return;

	const QPoint pos = getValue("position").toPoint();
	const QSize size = getValue("size").toSize();

	Phonon::VideoWidget::ScaleMode scaleMode = Phonon::VideoWidget::ScaleAndCrop;
	switch(getValue("scaleMode").toInt())
	{
		case 1:
			scaleMode = Phonon::VideoWidget::FitInView;
			break;
	}

	if(interactive)
	{
		MoviePlaceholderItem *item = new MoviePlaceholderItem(this);
		item->setRect(QRect(QPoint(), size));
		item->setPos(pos);

		item->setBrush(Qt::darkGray);
		item->setPen(QPen(Qt::black));

		QGraphicsPixmapItem *icon = new QGraphicsPixmapItem(item);
		icon->setPixmap(QIcon::fromTheme("applications-multimedia").pixmap(QSize(128, 128)));

		QGraphicsTextItem *label = new QGraphicsTextItem(item);
		label->setDefaultTextColor(Qt::black);
		label->setPlainText(getValue("src").toString());

		icon->setVisible(
			size.width() > icon->pixmap().size().width() &&
			size.height() > icon->pixmap().size().height()
		);

		icon->setPos(
			(size.width() - icon->pixmap().width()) / 2,
			(size.height() - icon->pixmap().height()) / 2
		);

		label->setTextWidth(size.width());
		label->setVisible(icon->isVisible());

		scene->addItem(item);
	}
	else
	{
		player = new Phonon::VideoPlayer(Phonon::VideoCategory);
		connect(player->mediaObject(), SIGNAL(aboutToFinish()), this, SLOT(restart()));
		player->load(getValue("src").toString());
		player->setVolume(getValue("volume", 100).toFloat() / 100);
		player->videoWidget()->setScaleMode(scaleMode);
		player->setFixedSize(size);

		QGraphicsProxyWidget *proxy = scene->addWidget(player);
		proxy->setPos(pos);
	}
}

PropertyList MovieElement::getProperties() const
{
	FilePropertyManager *fileManager = new FilePropertyManager;
	connect(fileManager, SIGNAL(modified(QString, QVariant)), this, SLOT(propertyChanged(QString, QVariant)));

	BoolPropertyManager *boolManager = new BoolPropertyManager;
	connect(boolManager, SIGNAL(modified(QString, QVariant)), this, SLOT(propertyChanged(QString, QVariant)));

	IntSliderPropertyManager *sliderManager = new IntSliderPropertyManager;
	connect(sliderManager, SIGNAL(modified(QString, QVariant)), this, SLOT(propertyChanged(QString, QVariant)));

	EnumPropertyManager *enumManager = new EnumPropertyManager;
	connect(enumManager, SIGNAL(modified(QString, QVariant)), this, SLOT(propertyChanged(QString, QVariant)));

	Property *group = new Property(0, tr("Vidéo"));

	Property *src = new Property(fileManager, tr("Source"), "src");
	src->setToolTip(tr("Chemin de la vidéo"));
	src->setValue(this->getValue("src"));
	fileManager->setRequired("src", true);
	fileManager->setFilter("src", MOVIE_FILTER);
	group->addProperty(src);

	Property *loop = new Property(boolManager, tr("Boucle"), "loop");
	loop->setToolTip(tr("Lire la vidéo en boucle"));
	loop->setValue(this->getValue("loop"));
	group->addProperty(loop);

	Property *volume = new Property(sliderManager, tr("Volume"), "volume");
	volume->setToolTip(tr("Volume de la vidéo"));
	volume->setValue(this->getValue("volume"));
	sliderManager->setMaximum("volume", 100);
	group->addProperty(volume);

	Property *scaleMode = new Property(enumManager, tr("Mise à l'échelle"), "scaleMode");
	scaleMode->setToolTip(tr("Mode de mise à l'échelle de la vidéo"));
	scaleMode->setValue(this->getValue("scaleMode"));
	enumManager->setEnumNames("scaleMode", QStringList() << tr("Remplir") << tr("Conserver"));
	group->addProperty(scaleMode);

	return PropertyList()
		<< SlideElement::getProperties()
		<< group;
}

void MovieElement::restart()
{
	if(getValue("loop").toBool())
	{
		player->mediaObject()->enqueue(Phonon::MediaSource(getValue("src").toString()));
	}
	else
		playbackFinished = true;
}

void MovieElement::play()
{
	if(!playbackFinished)
		player->play();
}

void MovieElement::pause()
{
	if(!playbackFinished)
		player->pause();
}

void MovieElement::stop()
{
	player->stop();
	playbackFinished = false;
}

void MovieElement::toggleMute()
{
	player->audioOutput()->setMuted(!player->audioOutput()->isMuted());
}

void MovieElement::destroy()
{
	player->deleteLater();
}

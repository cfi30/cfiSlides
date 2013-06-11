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

#include "videoelement.h"

VideoElement::VideoElement() : SlideElement()
{
	playbackFinished = false;
	setValue("size", QSize(600, 400));
	setValue("volume", 100);
}

QString VideoElement::previewUrl() const
{
	return getValue("src").toString();
}

void VideoElement::render(QGraphicsScene *scene, const bool interactive)
{
	SlideElement::render(scene, interactive);

	if(!getValue("visible").toBool())
		return;

	const QPoint pos = getValue("position").toPoint();
	const QSize size = getValue("size").toSize();
	Qt::AspectRatioMode scaleMode = Qt::KeepAspectRatioByExpanding;
	switch(getValue("scaleMode").toInt())
	{
		case 1:
			scaleMode = Qt::IgnoreAspectRatio;
			break;
		case 2:
			scaleMode = Qt::KeepAspectRatio;
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
		QGraphicsRectItem *container = new QGraphicsRectItem;
		container->setBrush(Qt::black);
		container->setPen(Qt::NoPen);
		container->setRect(QRect(QPoint(), size));
		container->setPos(pos);
		scene->addItem(container);

		QGraphicsVideoItem *item = new QGraphicsVideoItem(container);
		item->setSize(size);
		item->setAspectRatioMode(scaleMode);

		player = new QMediaPlayer;
		player->setVideoOutput(item);
		player->setVolume(getValue("volume").toInt());
		connect(player, SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(stateChanged(QMediaPlayer::State)));

		QMediaPlaylist *playlist = new QMediaPlaylist(player);
		playlist->addMedia(QUrl::fromLocalFile(getValue("src").toString()));
		if(getValue("loop").toBool())
			playlist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
		player->setPlaylist(playlist);
	}
}

PropertyList VideoElement::getProperties() const
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
	sliderManager->setSuffix("volume", tr(" %"));
	group->addProperty(volume);

	Property *scaleMode = new Property(enumManager, tr("Mise à l'échelle"), "scaleMode");
	scaleMode->setToolTip(tr("Mode de mise à l'échelle de la vidéo"));
	scaleMode->setValue(this->getValue("scaleMode"));
	enumManager->setEnumNames("scaleMode", QStringList() << tr("Remplir & Conserver") << tr("Remplir") << tr("Conserver"));
	group->addProperty(scaleMode);

	return PropertyList()
		<< SlideElement::getProperties()
		<< group;
}

void VideoElement::stateChanged(QMediaPlayer::State state)
{
	if(state == QMediaPlayer::StoppedState)
		playbackFinished = true;
}

void VideoElement::play()
{
	if(!playbackFinished)
	{
		qApp->processEvents();
		player->play();
	}
}

void VideoElement::pause()
{
	if(!playbackFinished)
	{
		qApp->processEvents();
		player->pause();
	}
}

void VideoElement::stop()
{
	player->stop();
	playbackFinished = false;
}

void VideoElement::toggleMute()
{
	player->setMuted(!player->isMuted());
}

void VideoElement::destroy()
{
	player->deleteLater();
}

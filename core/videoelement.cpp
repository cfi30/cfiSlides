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
	setValue(QStringLiteral("size"), QSize(600, 400));
	setValue(QStringLiteral("volume"), 100);
}

QString VideoElement::previewUrl() const
{
	return getValue(QStringLiteral("src")).toString();
}

void VideoElement::render(QGraphicsScene *scene, const bool interactive)
{
	SlideElement::render(scene, interactive);

	if(!getValue(QStringLiteral("visible")).toBool())
		return;

	const QPoint pos = getValue(QStringLiteral("position")).toPoint();
	const QSize size = getValue(QStringLiteral("size")).toSize();
	Qt::AspectRatioMode scaleMode = Qt::KeepAspectRatioByExpanding;
	switch(getValue(QStringLiteral("scaleMode")).toInt())
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
		label->setPlainText(getValue(QStringLiteral("src")).toString());

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
		player->setVolume(getValue(QStringLiteral("volume")).toInt());
		connect(player, &QMediaPlayer::stateChanged, this, &VideoElement::stateChanged);

		QMediaPlaylist *playlist = new QMediaPlaylist(player);
		playlist->addMedia(QUrl::fromLocalFile(getValue(QStringLiteral("src")).toString()));
		if(getValue(QStringLiteral("loop")).toBool())
			playlist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
		player->setPlaylist(playlist);
	}
}

PropertyList VideoElement::getProperties() const
{
	FilePropertyManager *fileManager = new FilePropertyManager;
	connect(fileManager, &PropertyManager::modified, this, &VideoElement::propertyChanged);

	BoolPropertyManager *boolManager = new BoolPropertyManager;
	connect(boolManager, &PropertyManager::modified, this, &VideoElement::propertyChanged);

	IntSliderPropertyManager *sliderManager = new IntSliderPropertyManager;
	connect(sliderManager, &PropertyManager::modified, this, &VideoElement::propertyChanged);

	EnumPropertyManager *enumManager = new EnumPropertyManager;
	connect(enumManager, &PropertyManager::modified, this, &VideoElement::propertyChanged);

	Property *group = new Property(0, tr("Vidéo"));

	Property *src = new Property(fileManager, tr("Source"), "src");
	src->setToolTip(tr("Chemin de la vidéo"));
	src->setValue(this->getValue(QStringLiteral("src")));
	fileManager->setRequired(QStringLiteral("src"), true);
	fileManager->setFilter(QStringLiteral("src"), MOVIE_FILTER);
	group->addProperty(src);

	Property *loop = new Property(boolManager, tr("Boucle"), "loop");
	loop->setToolTip(tr("Lire la vidéo en boucle"));
	loop->setValue(this->getValue(QStringLiteral("loop")));
	group->addProperty(loop);

	Property *volume = new Property(sliderManager, tr("Volume"), "volume");
	volume->setToolTip(tr("Volume de la vidéo"));
	volume->setValue(this->getValue(QStringLiteral("volume")));
	sliderManager->setMaximum(QStringLiteral("volume"), 100);
	sliderManager->setSuffix(QStringLiteral("volume"), tr(" %"));
	group->addProperty(volume);

	Property *scaleMode = new Property(enumManager, tr("Mise à l'échelle"), "scaleMode");
	scaleMode->setToolTip(tr("Mode de mise à l'échelle de la vidéo"));
	scaleMode->setValue(this->getValue(QStringLiteral("scaleMode")));
	enumManager->setEnumNames(QStringLiteral("scaleMode"), QStringList() << tr("Remplir & Conserver") << tr("Remplir") << tr("Conserver"));
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

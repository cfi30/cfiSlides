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

#include "audioelement.h"

AudioElement::AudioElement() : SlideElement()
{
	playbackFinished = false;
	setValue("volume", 100);
}

QString AudioElement::previewUrl() const
{
	return getValue("src").toString();
}

void AudioElement::render(QGraphicsScene *scene, const bool interactive)
{
	SlideElement::render(scene, interactive);

	if(interactive || !getValue("visible").toBool())
		return;

	player = new QMediaPlayer;
	player->setVolume(getValue("volume").toInt());
	connect(player, &QMediaPlayer::stateChanged, this, &AudioElement::stateChanged);

	QMediaPlaylist *playlist = new QMediaPlaylist(player);
	playlist->addMedia(QUrl::fromLocalFile(getValue("src").toString()));
	if(getValue("loop").toBool())
		playlist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
	player->setPlaylist(playlist);
}

PropertyList AudioElement::getProperties() const
{
	BoolPropertyManager *boolManager = new BoolPropertyManager;
	connect(boolManager, &PropertyManager::modified, this, &AudioElement::propertyChanged);

	FilePropertyManager *fileManager = new FilePropertyManager;
	connect(fileManager, &PropertyManager::modified, this, &AudioElement::propertyChanged);

	IntSliderPropertyManager *sliderManager = new IntSliderPropertyManager;
	connect(sliderManager, &PropertyManager::modified, this, &AudioElement::propertyChanged);

	Property *visible = new Property(boolManager, tr("Activer"), "visible");
	visible->setToolTip(tr("Activer l'élément"));
	visible->setValue(this->getValue("visible"));

	Property *group = new Property(0, tr("Son"));

	Property *src = new Property(fileManager, tr("Source"), "src");
	src->setToolTip(tr("Source du fichier audio"));
	src->setValue(this->getValue("src"));
	fileManager->setRequired("src", true);
	fileManager->setFilter("src", AUDIO_FILTER);
	group->addProperty(src);

	Property *loop = new Property(boolManager, tr("Boucle"), "loop");
	loop->setToolTip(tr("Lire le son en boucle"));
	loop->setValue(this->getValue("loop"));
	group->addProperty(loop);

	Property *volume = new Property(sliderManager, tr("Volume"), "volume");
	volume->setToolTip(tr("Volume deu son"));
	volume->setValue(this->getValue("volume"));
	sliderManager->setMaximum("volume", 100);
	sliderManager->setSuffix("volume", tr(" %"));
	group->addProperty(volume);

	return PropertyList()
		<< SlideshowElement::getProperties()
		<< visible
		<< group;
}

void AudioElement::stateChanged(QMediaPlayer::State state)
{
	if(state == QMediaPlayer::StoppedState)
		playbackFinished = true;
}

void AudioElement::play()
{
	if(!playbackFinished)
		player->play();
}

void AudioElement::pause()
{
	if(!playbackFinished)
		player->pause();
}

void AudioElement::stop()
{
	player->stop();
	playbackFinished = false;
}

void AudioElement::toggleMute()
{
	player->setMuted(!player->isMuted());
}

void AudioElement::destroy()
{
	player->deleteLater();
}

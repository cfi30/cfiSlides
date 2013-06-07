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

	mediaObject = new Phonon::MediaObject(this);
	mediaObject->setCurrentSource(Phonon::MediaSource(getValue("src").toString()));
	connect(mediaObject, SIGNAL(aboutToFinish()), this, SLOT(restart()));
	audioOutput = new Phonon::AudioOutput(Phonon::MusicCategory, mediaObject);
	Phonon::createPath(mediaObject, audioOutput);
	audioOutput->setVolume(getValue("volume", 100).toFloat() / 100);
}

PropertyList AudioElement::getProperties() const
{
	BoolPropertyManager *boolManager = new BoolPropertyManager;
	connect(boolManager, SIGNAL(modified(QString, QVariant)), this, SLOT(propertyChanged(QString, QVariant)));

	FilePropertyManager *fileManager = new FilePropertyManager;
	connect(fileManager, SIGNAL(modified(QString, QVariant)), this, SLOT(propertyChanged(QString, QVariant)));

	IntSliderPropertyManager *sliderManager = new IntSliderPropertyManager;
	connect(sliderManager, SIGNAL(modified(QString, QVariant)), this, SLOT(propertyChanged(QString, QVariant)));

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

void AudioElement::restart()
{
	if(getValue("loop").toBool())
	{
		mediaObject->enqueue(Phonon::MediaSource(getValue("src").toString()));
	}
	else
		playbackFinished = true;
}

void AudioElement::play()
{
	if(!playbackFinished)
		mediaObject->play();
}

void AudioElement::pause()
{
	if(!playbackFinished)
		mediaObject->pause();
}

void AudioElement::stop()
{
	mediaObject->stop();
	playbackFinished = false;
}

void AudioElement::toggleMute()
{
	if(!audioOutput->isMuted())
		audioOutput->setMuted(true);
	else
	{
		audioOutput->setMuted(false);
		// setMuted(false) does not take the sound back without setVolume
		audioOutput->setVolume(getValue("volume", 100).toFloat() / 100);
	}
}

void AudioElement::destroy()
{
	mediaObject->deleteLater();
}

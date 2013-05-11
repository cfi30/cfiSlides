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

void AudioElement::bindProperties(QtTreePropertyBrowser *browser) const
{
	SlideshowElement::bindProperties(browser);

	QtBoolPropertyManager *boolManager = new QtBoolPropertyManager();
	QtCheckBoxFactory *checkBoxFactory = new QtCheckBoxFactory(browser);
	QtGroupPropertyManager *groupManager = new QtGroupPropertyManager();
	FilePathManager *filePathManager = new FilePathManager();
	FileEditFactory *fileEditFactory = new FileEditFactory(browser);
	QtIntPropertyManager *intManager = new QtIntPropertyManager();
	QtSliderFactory *sliderFactory = new QtSliderFactory(browser);

	QtProperty *visible = boolManager->addProperty(tr("Activer"));
	visible->setWhatsThis("visible");
	visible->setToolTip(tr("Activer l'élément"));
	boolManager->setValue(visible, getValue("visible").toBool());
	browser->addProperty(visible);

	QtProperty *group = groupManager->addProperty(tr("Son"));
	group->setModified(true);

	QtProperty *src = filePathManager->addProperty(tr("Source"));
	src->setWhatsThis("src");
	src->setToolTip(tr("Chemin du son"));
	filePathManager->setValue(src, getValue("src").toString());
	filePathManager->setFilter(src, AUDIO_FILTER);
	group->addSubProperty(src);

	QtProperty *loop = boolManager->addProperty(tr("Boucle"));
	loop->setWhatsThis("loop");
	loop->setToolTip(tr("Lire la vidéo en boucle"));
	boolManager->setValue(loop, getValue("loop").toBool());
	group->addSubProperty(loop);

	QtProperty *volume = intManager->addProperty(tr("Volume"));
	volume->setWhatsThis("volume");
	volume->setToolTip(tr("Volume de la vidéo"));
	intManager->setValue(volume, getValue("volume", 100).toInt());
	intManager->setMinimum(volume, 0);
	intManager->setMaximum(volume, 100);
	group->addSubProperty(volume);

	browser->addProperty(group);

	browser->setFactoryForManager(boolManager, checkBoxFactory);
	browser->setFactoryForManager(filePathManager, fileEditFactory);
	browser->setFactoryForManager(intManager, sliderFactory);

	connect(boolManager, SIGNAL(valueChanged(QtProperty*,bool)), this, SLOT(boolValueChanged(QtProperty*,bool)));
	connect(filePathManager, SIGNAL(valueChanged(QtProperty*,QString)), this, SLOT(stringValueChanged(QtProperty*,QString)));
	connect(intManager, SIGNAL(valueChanged(QtProperty*,int)), this, SLOT(intValueChanged(QtProperty*,int)));
}

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
		if(loadedFile != getValue("src").toString())
		{
			// render video widget on the scene. will be painted in a GraphicsPixmapItem later (this is a really dirty hack...)
			player = new Phonon::VideoPlayer(Phonon::VideoCategory);
			connect(player->mediaObject(), SIGNAL(hasVideoChanged(bool)), this, SLOT(interactiveVideoLoaded(bool)));
			connect(player->mediaObject(), SIGNAL(finished()), this, SLOT(takeSnapshot()));
			player->audioOutput()->setMuted(true);
			player->play(getValue("src").toString());
			player->videoWidget()->setScaleMode(scaleMode);
			player->setFixedSize(size);

			QGraphicsProxyWidget *proxy = scene->addWidget(player);
			proxy->setPos(QPoint());
			//proxy->setPos(pos); // FIXME: the snapshot is taken from the hole scene in takeSnapshot() so the correct position must not be set now.
			proxy->setData(Qt::UserRole, getIndex());
		}
		else
		{
			GraphicsPixmapItem *item = new GraphicsPixmapItem();
			item->setPixmap(snapshot.scaled(size));
			item->setPos(pos);
			item->setData(Qt::UserRole, getIndex());
			item->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemSendsGeometryChanges);
			item->setElement(this);
			scene->addItem(item);
		}
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

void MovieElement::interactiveVideoLoaded(bool)
{
	loadedFile = getValue("src").toString();
	player->seek(1000);
	player->pause();

	QTimer::singleShot(MOVIE_PAINT_DELAY, this, SLOT(takeSnapshot()));
}

void MovieElement::takeSnapshot()
{
	// FIXME: Take a picture of the movie alone.
	snapshot = QPixmap(getValue("size").toSize());

	QPainter painter(&snapshot);
	painter.setRenderHint(QPainter::Antialiasing);
	scene->render(&painter, scene->sceneRect());

	player->deleteLater();
	emit refresh();
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

void MovieElement::bindProperties(QtTreePropertyBrowser *browser) const
{
	SlideElement::bindProperties(browser);

	QtGroupPropertyManager *groupManager = new QtGroupPropertyManager();
	FilePathManager *filePathManager = new FilePathManager();
	FileEditFactory *fileEditFactory = new FileEditFactory(browser);
	QtBoolPropertyManager *boolManager = new QtBoolPropertyManager();
	QtCheckBoxFactory *checkBoxFactory = new QtCheckBoxFactory(browser);
	QtIntPropertyManager *intManager = new QtIntPropertyManager();
	QtSliderFactory *sliderFactory = new QtSliderFactory(browser);
	QtEnumPropertyManager *enumManager = new QtEnumPropertyManager();
	QtEnumEditorFactory *enumEditorFactory = new QtEnumEditorFactory(browser);

	QtProperty *group = groupManager->addProperty(tr("Vidéo"));
	group->setModified(true);

	QtProperty *src = filePathManager->addProperty(tr("Source"));
	src->setWhatsThis("src");
	src->setToolTip(tr("Chemin de la vidéo"));
	filePathManager->setValue(src, getValue("src").toString());
	filePathManager->setFilter(src, MOVIE_FILTER);
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

	QtProperty *scalingMode = enumManager->addProperty(tr("Mise à l'échelle"));
	scalingMode->setWhatsThis("scaleMode");
	scalingMode->setToolTip(tr("Mode de mise à l'échelle de la vidéo"));
	enumManager->setEnumNames(scalingMode, QStringList() << tr("Remplir") << tr("Conserver"));
	enumManager->setValue(scalingMode, getValue("scaleMode").toInt());
	group->addSubProperty(scalingMode);

	browser->addProperty(group);

	browser->setFactoryForManager(filePathManager, fileEditFactory);
	browser->setFactoryForManager(boolManager, checkBoxFactory);
	browser->setFactoryForManager(intManager, sliderFactory);
	browser->setFactoryForManager(enumManager, enumEditorFactory);

	connect(filePathManager, SIGNAL(valueChanged(QtProperty*,QString)), this, SLOT(stringValueChanged(QtProperty*,QString)));
	connect(boolManager, SIGNAL(valueChanged(QtProperty*,bool)), this, SLOT(boolValueChanged(QtProperty*,bool)));
	connect(intManager, SIGNAL(valueChanged(QtProperty*,int)), this, SLOT(intValueChanged(QtProperty*,int)));
	connect(enumManager, SIGNAL(valueChanged(QtProperty*,int)), this, SLOT(intValueChanged(QtProperty*,int)));
}

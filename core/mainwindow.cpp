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

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QString commandLineHelp, QString openFile, bool disablePlugins, QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	registerElementType(SlideElementType(
		qRegisterMetaType<RectElement>(), tr("Rectangle"), QIcon::fromTheme("insert-rectangle")
	));
	registerElementType(SlideElementType(
		qRegisterMetaType<EllipseElement>(), tr("Ellipse"), QIcon::fromTheme("insert-ellipse")
	));
	registerElementType(SlideElementType(
		qRegisterMetaType<LineElement>(), tr("Ligne"), QIcon::fromTheme("insert-horizontal-rule")
	));
	registerElementType(SlideElementType(
		qRegisterMetaType<TextElement>(), tr("Texte"), QIcon::fromTheme("insert-text")
	));
	registerElementType(SlideElementType(
		qRegisterMetaType<ImageElement>(), tr("Image"), QIcon::fromTheme("insert-image")
	));
	registerElementType(SlideElementType(
		qRegisterMetaType<VideoElement>(), tr("Vidéo"), QIcon::fromTheme("insert-video")
	));
	registerElementType(SlideElementType(
		qRegisterMetaType<AudioElement>(), tr("Son"), QIcon::fromTheme("insert-audio")
	));

	new QShortcut(QKeySequence("Shift+Tab"), this, SLOT(selectNextSlide()));
	new QShortcut(QKeySequence("Ctrl+Shift+Tab"), this, SLOT(selectPrevSlide()));

	this->restoreGeometry(QSettings().value("mainWindow/geometry").toByteArray());
	this->restoreState(QSettings().value("mainWindow/state").toByteArray());

	ui->menuRecentFiles->setDisabled(QSettings().value("recentFiles").toStringList().isEmpty());

	ui->actionToolbar->setChecked(ui->toolBar->isVisibleTo(this));
	ui->actionSlideList->setChecked(ui->slideListDock->isVisibleTo(this));
	ui->actionSlideDock->setChecked(ui->slideDock->isVisibleTo(this));
	ui->actionProperties->setChecked(ui->propertiesDock->isVisibleTo(this));
	ui->actionMediaDock->setChecked(ui->mediaDock->isVisibleTo(this));

	ui->toolBar->setWindowTitle(ui->actionToolbar->text());
	ui->slideListDock->setWindowTitle(ui->actionSlideList->text());
	ui->slideDock->setWindowTitle(ui->actionSlideDock->text());
	ui->propertiesDock->setWindowTitle(ui->actionProperties->text());
	ui->mediaDock->setWindowTitle(ui->actionMediaDock->text());

	// this doesn't work when using the designer
	connect(ui->menuRecentFiles, &QMenu::aboutToShow, this, &MainWindow::displayRecentFiles);
	connect(ui->menuRecentFiles, &QMenu::triggered, this, &MainWindow::openRecentFile);

	ui->actionCurrentSlide->setSeparator(true);

	connect(ui->menuInsertElement, &QMenu::aboutToShow, this, &MainWindow::displayInsertElemMenu);

	currentSlideActions = new QActionGroup(this);
	currentSlideActions->addAction(ui->menuInsertElement->menuAction());
	currentSlideActions->addAction(ui->menuSelectedElements->menuAction());
	currentSlideActions->addAction(ui->actionCurrentSlide);
	currentSlideActions->addAction(ui->actionSelectAll);
	currentSlideActions->addAction(ui->actionRenameSlide);
	currentSlideActions->addAction(ui->actionDuplicateSlide);
	currentSlideActions->addAction(ui->actionRepaint);
	currentSlideActions->addAction(ui->actionMoveSlideLeft);
	currentSlideActions->addAction(ui->actionMoveSlideRight);
	currentSlideActions->addAction(ui->actionDeleteSlide);

	selectionActions = new QActionGroup(this);
	selectionActions->addAction(ui->actionDeleteElements);
	selectionActions->addAction(ui->actionDuplicateElements);
	selectionActions->addAction(ui->actionRaiseElement);
	selectionActions->addAction(ui->actionLowerElement);
	selectionActions->addAction(ui->actionBringElementToFront);
	selectionActions->addAction(ui->actionBringElementToBack);

	previewPlayer = new QMediaPlayer(this);
	previewPlayer->setNotifyInterval(REFRESH_INTERVAL);
	previewPlayer->setVideoOutput(ui->videoPlayer);
	previewPlayer->setVolume(QSettings().value("previewVolume", 100).toInt());
	ui->volumeSlider->setValue(previewPlayer->volume());
	ui->pauseButton->hide();

	connect(previewPlayer, &QMediaPlayer::seekableChanged, this, &MainWindow::previewSeekableChanged);
	connect(previewPlayer, &QMediaPlayer::durationChanged, this, &MainWindow::previewDurationChanged);
	connect(previewPlayer, &QMediaPlayer::positionChanged, this, &MainWindow::previewPositionChanged);
	connect(previewPlayer, &QMediaPlayer::stateChanged, this, &MainWindow::previewStateChanged);

	this->slideshow = 0;
	this->newSlideshowCount = 0;
	this->commandLineHelp = commandLineHelp;

	if(openFile.isEmpty() || !openSlideshow(openFile))
	{
		newSlideshow();
		statusBar()->showMessage(tr("Merci d'utiliser %1 !").arg(qApp->applicationName()), STATUS_TIMEOUT);
	}

	moveFinishTimer.setInterval(REFRESH_INTERVAL);
	moveFinishTimer.setSingleShot(true);
	connect(&moveFinishTimer, &QTimer::timeout, this, &MainWindow::moveFinishTimerTimeout);

	if(disablePlugins)
	{
		ui->actionPlugins->setEnabled(false);
		ui->actionAboutPlugins->setEnabled(false);
		statusBar()->showMessage(tr("Extensions désactivés par la ligne de commandes."));
	}
	else
		QTimer::singleShot(REFRESH_INTERVAL, this, SLOT(loadPlugins()));
}

MainWindow::~MainWindow()
{
	unloadPlugins();

	delete ui;
}

Slideshow *MainWindow::getSlideshow() const
{
	return slideshow;
}

void MainWindow::setWindowModified(const bool modified)
{
	QMainWindow::setWindowModified(modified);
	ui->actionSave->setEnabled(modified);
}

void MainWindow::setWindowTitle(const QString &fileName)
{
	QMainWindow::setWindowTitle(tr("%1[*] - %2").arg(fileName, qApp->applicationName()));
}

void MainWindow::newSlideshow()
{
	if(this->slideshow != 0)
	{
		if(!closeSlideshow())
			return;
	}

	this->setWindowTitle(tr("Nouveau diaporama %1").arg(++newSlideshowCount));

	this->slideshow = new Slideshow();
	createEmptySlide();

	this->setWindowModified(false);
}

bool MainWindow::openSlideshow(const QString knowPath)
{
	QString newFile;
	if(knowPath.isEmpty())
	{
		newFile = QFileDialog::getOpenFileName(this, ui->actionOpen->text(), QDir::homePath(), FILE_FILTER);
		if(newFile.isEmpty())
			return false;
	}
	else if(!QFile::exists(knowPath))
	{
		QMessageBox::critical(this, ui->actionOpen->text(), tr("Impossible d'ouvrir le diaporama : le fichier demandé n'existe pas."));
		return false;
	}
	else
		newFile = knowPath;

	QFile file(newFile);
	if(!file.open(QIODevice::ReadOnly))
	{
		QMessageBox::critical(this, ui->actionOpen->text(), tr("Impossible de lire le contenu du fichier."));
		return false;
	}

	int errorsCount = 0;

	QDataStream in(&file);
	QString fileAppName;
	in >> fileAppName;
	if(fileAppName != qApp->applicationName())
	{
		QMessageBox::critical(this, ui->actionOpen->text(), tr("Le fichier demandé ne peut pas être ouvert avec %1.").arg(qApp->applicationName()));
		return false;
	}

	if(this->slideshow != 0)
	{
		if(!closeSlideshow())
			return false;
	}

	QVariantMap metadata;
	in >> metadata;

	this->slideshow = new Slideshow();
	this->slideshow->setValues(metadata);
	this->currentSlideActions->setEnabled(false);

	int slidesCount = 0;
	in >> slidesCount;

	QProgressDialog *progress = new QProgressDialog(this);
	progress->setWindowTitle(ui->actionOpen->text());
	progress->setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
	progress->setCancelButtonText(QString());
	progress->setLabelText(tr("Chargement et affichage du diaporama en cours..."));
	progress->setMaximum(slidesCount);
	progress->open();

	for(int si = 0; si < slidesCount; si++)
	{
		progress->setValue(si + 1);

		QVariantMap properties;
		in >> properties;

		Slide *slide = this->slideshow->createSlide(QString::number(si));
		slide->setValues(properties);

		int elementsCount = 0;
		in >> elementsCount;
		for(int ei = 0; ei < elementsCount; ei++)
		{
			char *type;
			in >> type;

			QVariantMap properties;
			in >> properties;

			SlideElement *element = (SlideElement *)QMetaType::create(QMetaType::type(type));
			if(element == 0)
			{
				if(errorsCount == 0)
				{
					QMessageBox::warning(this, ui->actionOpen->text(),
						tr("Une erreur s'est produite lors du chargement des éléments graphiques de la diapositive n° %1 (index %2, élément %3, %4). L'élément en question a été supprimé du diaporama. Les erreurs suivantes seront ignorées.")
							.arg(si+1).arg(si).arg(ei)
							.arg(QString(type)=="" ? tr("Type inconnu") : type)
					);
					this->setWindowModified(true);
				}

				errorsCount++;
				continue;
			}

			element->setValues(properties);
			slide->addElement(element);
		}

		displaySlide(slide);
	}
	file.close();

	QString fileName = QFileInfo(newFile).fileName();
	this->setWindowTitle(fileName);
	this->setWindowFilePath(newFile);

	QStringList recentFiles = QSettings().value("recentFiles").toStringList();
	recentFiles.insert(0, newFile);
	recentFiles.removeDuplicates();
	if(recentFiles.count() > RECENT_FILES_MAX)
		recentFiles.removeLast();

	QSettings().setValue(QStringLiteral("recentFiles"), recentFiles);
	ui->menuRecentFiles->setEnabled(true);

	progress->close();
	progress->deleteLater();

	statusBar()->showMessage(tr("Fin du chargement de %1. Diapositives : %2 | Erreurs : %3").arg(fileName).arg(slidesCount).arg(errorsCount), STATUS_TIMEOUT);
	if(slidesCount == 0)
		return true;

	QListWidgetItem *item = ui->slideList->item(0);
	ui->slideList->scrollToItem(item);
	ui->slideList->setCurrentItem(item);

	return true;
}

bool MainWindow::saveSlideshow()
{
	if(this->windowFilePath().isEmpty())
	{
		return saveSlideshowAs();
	}

	QFile file(this->windowFilePath());
	if(!file.open(QIODevice::WriteOnly))
	{
		QMessageBox::critical(this, ui->actionSave->text(), tr("Impossible d'écrire dans le fichier."));
		return false;
	}

	QDataStream out(&file);

	QList<Slide *> slides = slideshow->getSlides();
	out << qApp->applicationName();
	out << slideshow->getValues();
	out << slides.size();
	foreach(Slide *slide, slides)
	{
		out << slide->getValues();
		QList<SlideElement *> elements = slide->getElements();
		out << elements.size();
		foreach(SlideElement *element, elements)
		{
			out << element->type();
			out << element->getValues();
		}
	}
	file.close();

	this->setWindowModified(false);
	statusBar()->showMessage(tr("%1 diapositive(s) enregistrée(s) dans %2.").arg(slides.size()).arg(this->windowFilePath()), STATUS_TIMEOUT);
	return true;
}

bool MainWindow::saveSlideshowAs()
{
	const QString newFile = QFileDialog::getSaveFileName(this, ui->actionSave_as->text(), QDir::homePath(), FILE_FILTER);
	if(newFile.isEmpty())
		return false;

	this->setWindowTitle(QFileInfo(newFile).fileName());
	this->setWindowFilePath(newFile);
	return saveSlideshow();
}

bool MainWindow::closeSlideshow()
{
	if(this->isWindowModified())
	{
		const int choice = QMessageBox::warning(this, tr("Sauvegarder les changements"), tr("Voulez-vous enregistrer les changements apportés à ce diaporama avant de le fermer ?"), QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel, QMessageBox::Cancel);
		switch(choice)
		{
			case QMessageBox::Save:
				if(!saveSlideshow())
					return false;
				break;
			case QMessageBox::Cancel:
				return false;
		}
	}

	statusBar()->showMessage(tr("Fermeture du diaporama..."));

	QMainWindow::setWindowTitle(QString("[*]%1").arg(qApp->applicationName()));
	this->setWindowModified(false);
	delete this->slideshow;

	ui->slideList->clear();
	ui->slideTree->clear();
	ui->propertiesEditor->clear();
	updateMediaPreview();

	while(ui->displayWidget->count() > 0)
	{
		GraphicsView *view = qobject_cast<GraphicsView *>(ui->displayWidget->widget(0));
		view->scene()->blockSignals(true);
		delete view->scene();
		ui->displayWidget->removeWidget(view);
		delete view;
	}

	statusBar()->clearMessage();

	this->setWindowFilePath(QString());
	return true;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	if(!closeSlideshow())
		return event->ignore();

	QSettings().setValue(QStringLiteral("mainWindow/geometry"), this->saveGeometry());
	QSettings().setValue(QStringLiteral("mainWindow/state"), this->saveState());

	QMainWindow::closeEvent(event);
}

void MainWindow::createEmptySlide()
{
	Slide *slide = this->slideshow->createSlide(tr("Diapositive %1").arg(this->slideshow->getSlides().size() + 1));
	displaySlide(slide);

	if(ui->slideList->count() > 1)
	{
		QListWidgetItem *item = ui->slideList->item(ui->slideList->count() - 1);
		ui->slideList->scrollToItem(item);
		ui->slideList->setCurrentItem(item);
	}

	setWindowModified(true);
	statusBar()->showMessage(tr("Diapositive créée."), STATUS_TIMEOUT);
}

void MainWindow::displaySlide(Slide *slide)
{
	statusBar()->showMessage(tr("Affichage de %1...").arg(slide->getValue(QStringLiteral("name")).toString()));

	QGraphicsScene *scene = new QGraphicsScene();
	scene->setSceneRect(slideshow->getValue("geometry", QDesktopWidget().screenGeometry()).toRect());
	scene->setItemIndexMethod(QGraphicsScene::NoIndex);
	connect(scene, &QGraphicsScene::selectionChanged, this, &MainWindow::updateCurrentSlideTree);
	connect(scene, &QGraphicsScene::selectionChanged, this, &MainWindow::updateSelectionActions);
	connect(scene, &QGraphicsScene::selectionChanged, this, &MainWindow::updateCurrentPropertiesEditor);
	connect(scene, &QGraphicsScene::selectionChanged, this, &MainWindow::updateMediaPreview);

	GraphicsView *view = new GraphicsView(scene);
	view->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(view, &QWidget::customContextMenuRequested, this, &MainWindow::displayViewContextMenu);
	ui->displayWidget->addWidget(view);

	QPixmap pixmap(scene->sceneRect().size().toSize());
	QPainter painter(&pixmap);
	painter.setRenderHint(QPainter::Antialiasing);

	slide->render(scene, true);
	scene->render(&painter, scene->sceneRect());

	const int slideIndex = ui->slideList->count();
	const int currentRow = ui->slideList->currentRow();
	const int keepStart = currentRow - (MAX_LOADED_SLIDES / 2);
	const int keepEnd = currentRow + (MAX_LOADED_SLIDES / 2);
	if(slideIndex < keepStart || slideIndex > keepEnd)
		scene->clear();

	QIcon icon(pixmap.scaledToWidth(ui->slideList->iconSize().width()));
	QListWidgetItem *newItem = new QListWidgetItem(icon, slide->getValue(QStringLiteral("name")).toString());
	newItem->setFlags(newItem->flags() ^ Qt::ItemIsEditable);
	ui->slideList->addItem(newItem);

	if(currentRow == -1)
		ui->slideList->setCurrentRow(0);

	this->currentSlideActions->setEnabled(true);

	connect(slide, &SlideshowElement::modified, this, &MainWindow::slideModified);
	connect(slide, &Slide::moved, this, &MainWindow::slideElementMoved);
	connect(slide, &Slide::refresh, this, &MainWindow::reRenderSlide);

	statusBar()->clearMessage();
}

void MainWindow::updateSlide(const int index)
{
	Slide *slide = this->slideshow->getSlide(index);

	GraphicsView *view = qobject_cast<GraphicsView *>(ui->displayWidget->widget(index));
	view->scene()->blockSignals(true);
	view->scene()->clear();
	view->scene()->blockSignals(false);
	slide->render(view->scene(), true);

	ui->slideList->blockSignals(true);
	ui->slideList->item(index)->setText(slide->getValue(QStringLiteral("name")).toString());
	ui->slideList->blockSignals(false);

	updateIcon(index);
}

void MainWindow::updateIcon(const int index)
{
	GraphicsView *view = qobject_cast<GraphicsView *>(ui->displayWidget->widget(index));

	QPixmap pixmap(view->scene()->sceneRect().size().toSize());
	QPainter painter(&pixmap);
	painter.setRenderHint(QPainter::Antialiasing);
	view->scene()->render(&painter, view->scene()->sceneRect());

	QIcon icon(pixmap.scaledToWidth(ui->slideList->iconSize().width()));

	ui->slideList->blockSignals(true);
	ui->slideList->item(index)->setIcon(icon);
	ui->slideList->blockSignals(false);
}

void MainWindow::updateSlideTree(const int index)
{
	Slide *slide = this->slideshow->getSlide(index);

	ui->slideTree->blockSignals(true);
	ui->slideTree->clear();

	QTreeWidgetItem *topLevel = new QTreeWidgetItem(ui->slideTree);
	topLevel->setText(0, slide->getValue(QStringLiteral("name")).toString());
	topLevel->setData(0, Qt::UserRole, index);
	topLevel->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
	topLevel->setExpanded(true);

	QList<SlideElement *> elements = slide->getElements();
	QListIterator<SlideElement *> iterator(elements);
	iterator.toBack();
	while(iterator.hasPrevious()) // reverse foreach?
	{
		SlideElement *element = iterator.previous();

		const int elementIndex = elements.indexOf(element);
		QTreeWidgetItem *treeItem = new QTreeWidgetItem(topLevel);
		treeItem->setText(0, element->getValue(QStringLiteral("name")).toString());
		treeItem->setData(0, Qt::UserRole, elementIndex);
		treeItem->setFlags(treeItem->flags() | Qt::ItemIsEditable);

		QGraphicsItem *graphicsItem = sceneItemFromIndex(elementIndex);
		if(graphicsItem != 0 && graphicsItem->isSelected())
			treeItem->setSelected(true);
	}
	ui->slideTree->blockSignals(false);
}

void MainWindow::updateCurrentSlideTree()
{
	updateSlideTree(ui->slideList->currentRow());
}

void MainWindow::updatePropertiesEditor(const SlideshowElement *element)
{
	ui->propertiesEditor->clear();
	ui->propertiesEditor->addTopLevelProperties(element->getProperties());
}

void MainWindow::updateCurrentPropertiesEditor()
{
	if(ui->slideList->currentRow() == -1)
		return;

	Slide *slide = this->slideshow->getSlide(ui->slideList->currentRow());
	QTreeWidgetItem *item = 0;
	if(ui->slideTree->selectedItems().size() > 0)
		item = ui->slideTree->selectedItems()[0];

	if(item == 0 || item->parent() == 0)
		updatePropertiesEditor(slide);
	else
		updatePropertiesEditor(slide->getElement(item->data(0, Qt::UserRole).toInt()));
}

void MainWindow::updateSelectionActions()
{
	selectionActions->setDisabled(ui->slideTree->selectedItems().isEmpty());

	const int index = ui->slideList->currentRow();
	if(index == -1)
		return;

	Slide *slide = this->slideshow->getSlide(index);

	const int selectedItemsCount = ui->slideTree->selectedItems().size();
	const bool enableUpDown = slide->getElements().size() > 1 && selectedItemsCount == 1;

	ui->actionRaiseElement->setEnabled(enableUpDown && ui->slideTree->selectedItems()[0] != ui->slideTree->topLevelItem(0)->child(0));
	ui->actionLowerElement->setEnabled(enableUpDown && selectedItemsCount > 0 && ui->slideTree->selectedItems()[selectedItemsCount - 1] != ui->slideTree->topLevelItem(0)->child(ui->slideTree->topLevelItem(0)->childCount() - 1));

	ui->actionBringElementToFront->setEnabled(ui->actionRaiseElement->isEnabled());
	ui->actionBringElementToBack->setEnabled(ui->actionLowerElement->isEnabled());
}

void MainWindow::updateMediaPreview()
{
	previewPlayer->stop();
	ui->mediaPreview->setEnabled(false);
	ui->seekSlider->setValue(0);
	ui->durationLabel->setText(msToString(0));

	if(ui->slideTree->selectedItems().size() < 1 || !ui->mediaDock->isVisible())
		return;

	Slide *slide = this->slideshow->getSlide(ui->slideList->currentRow());
	QTreeWidgetItem *item = ui->slideTree->selectedItems()[0];
	SlideElement *element = slide->getElement(item->data(0, Qt::UserRole).toInt());
	QString previewUrl = element->previewUrl();

	if(!previewUrl.isEmpty())
	{
		previewPlayer->setMedia(QUrl::fromLocalFile(previewUrl));
		previewPlayer->play();
		ui->mediaPreview->setEnabled(true);
	}
}

void MainWindow::renameSlide()
{
	const int index = ui->slideList->currentRow();
	Slide *slide = this->slideshow->getSlide(index);
	bool ok;
	QString newName = QInputDialog::getText(this, ui->actionRenameSlide->text(), tr("Nouveau nom pour cette diapositive :"), QLineEdit::Normal, slide->getValue(QStringLiteral("name")).toString(), &ok);
	if(!ok || !validateSlideName(newName)) return;

	slide->setValue(QStringLiteral("name"), newName);
	updateSlide(index);
	updateSlideTree(index);
	updateCurrentPropertiesEditor();
	setWindowModified(true);
}

bool MainWindow::validateSlideName(const QString &name)
{
	if(name.trimmed().isEmpty())
	{
		QMessageBox::critical(this, tr("Renommer la diapositive"), tr("Le nom de la diapositive ne peut pas être vide."));
		return false;
	}

	return true;
}

bool MainWindow::validateElementName(const QString &name)
{
	if(name.trimmed().isEmpty())
	{
		QMessageBox::critical(this, tr("Renommer l'élément"), tr("Le nom de l'élément ne peut pas être vide."));
		return false;
	}
	return true;
}

QGraphicsItem *MainWindow::sceneItemFromIndex(const int index) const
{
	GraphicsView *view = qobject_cast<GraphicsView *>(ui->displayWidget->currentWidget());
	foreach(QGraphicsItem *item, view->scene()->items())
	{
		bool valid;
		if(item->data(Qt::UserRole).toInt(&valid) == index && valid)
			return item;
	}

	return 0;
}

void MainWindow::reRenderSlide()
{
	int index = ui->slideList->currentRow();
	Slide *slide = qobject_cast<Slide *>(sender());
	if(slide != 0)
		index = this->slideshow->indexOf(slide);

	updateSlide(index);
}

void MainWindow::deleteSlide()
{
	const int choice = QMessageBox::warning(this, ui->actionDeleteSlide->text(), tr("Êtes-vous certain de vouloir supprimer cette diapositive ?"), QMessageBox::Yes, QMessageBox::Cancel);
	if(choice == QMessageBox::Cancel)
		return;

	int index = ui->slideList->currentRow();
	ui->slideTree->clear();
	ui->propertiesEditor->clear();
	this->slideshow->removeSlide(index);

	ui->slideList->blockSignals(true);
	delete ui->slideList->takeItem(index);
	ui->slideList->blockSignals(false);

	GraphicsView *view = qobject_cast<GraphicsView *>(ui->displayWidget->currentWidget());
	ui->displayWidget->removeWidget(view);
	delete view->scene();
	delete view;

	if(this->slideshow->getSlides().size() > 0)
		currentSlideChanged(ui->slideList->currentRow());
	else
	{
		this->currentSlideActions->setEnabled(false);
		updateSelectionActions();
	}

	setWindowModified(true);
	statusBar()->showMessage(tr("Diapositive supprimée."), STATUS_TIMEOUT);
}

void MainWindow::slideModified()
{
	const int index = ui->slideList->currentRow();
	GraphicsView *view = qobject_cast<GraphicsView *>(ui->displayWidget->currentWidget());
	updateSlide(index);
	view->scene()->blockSignals(true);
	if(!ui->slideTree->selectedItems().isEmpty())
	{
		QGraphicsItem *item = sceneItemFromIndex(ui->slideTree->selectedItems()[0]->data(0, Qt::UserRole).toInt());
		if(item != 0) item->setSelected(true);
	}
	updateSlideTree(index);
	view->scene()->blockSignals(false);
	setWindowModified(true);
}

void MainWindow::slideElementMoved()
{
	moveFinishTimer.start();
	setWindowModified(true);
}

void MainWindow::about()
{
	QMessageBox::about(this, tr("À propos de %1 - %2").arg(qApp->applicationName()).arg(qApp->applicationPid()), QString(
		QString("<p><strong>%1 %2</strong><br /><kbd>%3</kbd></p>") +

		"<p>Copyright (C) 2013  Christian Fillion</p>" +

		"<p>%1 is free software: you can redistribute it and/or modify" +
		"   it under the terms of the GNU General Public License as published by" +
		"   the Free Software Foundation, either version 3 of the License, or" +
		"   (at your option) any later version.</p>" +

		"<p>%1 is distributed in the hope that it will be useful," +
		"   but WITHOUT ANY WARRANTY; without even the implied warranty of" +
		"   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the" +
		"   GNU General Public License for more details.</p>" +

		"<p>You should have received a copy of the GNU General Public License along with %1." +
		"   If not, see <a href=\"http://www.gnu.org/licenses/\">http://www.gnu.org/licenses/</a>.</p>"
	).arg(qApp->applicationName(), qApp->applicationVersion(), commandLineHelp));
}

void MainWindow::currentSlideChanged(int currentRow)
{
	if(currentRow == -1)
		return;

	const int slideCount = this->slideshow->getSlides().size();
	ui->displayWidget->setCurrentIndex(currentRow);
	ui->actionMoveSlideLeft->setEnabled(slideCount > 0 && currentRow != 0);
	ui->actionMoveSlideRight->setEnabled(slideCount > 0 && currentRow != slideCount - 1);

	updateSlideTree(currentRow);
	updateCurrentPropertiesEditor();
	updateMediaPreview();
	updateSelectionActions();

	const int keepStart = currentRow - (MAX_LOADED_SLIDES / 2);
	const int keepEnd = currentRow + (MAX_LOADED_SLIDES / 2);
	for(int index = 0; index < slideCount; index++)
	{
		const GraphicsView *view = qobject_cast<GraphicsView *>(ui->displayWidget->widget(index));
		if(index < keepStart || index > keepEnd)
			view->scene()->clear();
		else if(view->scene()->items().size() == 0)
		{
			Slide *slide = this->slideshow->getSlide(index);
			slide->render(view->scene(), true);
		}
	}
}

void MainWindow::slideItemChanged(QListWidgetItem *item)
{
	const int index = ui->slideList->row(item);
	Slide *slide = this->slideshow->getSlide(index);
	if(!validateSlideName(item->text()))
	{
		ui->slideList->blockSignals(true);
		ui->slideList->item(index)->setText(slide->getValue(QStringLiteral("name")).toString());
		ui->slideList->blockSignals(false);
		return;
	}

	slide->setValue(QStringLiteral("name"), item->text());
	updateSlide(index);
	updateSlideTree(index);
	updateCurrentPropertiesEditor();
	setWindowModified(true);
}

void MainWindow::elementItemChanged(QTreeWidgetItem *item, int)
{
	const int index = item->data(0, Qt::UserRole).toInt();
	if(item->parent() == 0)
	{
		Slide *slide = this->slideshow->getSlide(index);
		if(!validateSlideName(item->text(0)))
			return updateSlideTree(index);

		slide->setValue(QStringLiteral("name"), item->text(0));
		ui->slideList->item(index)->setText(item->text(0));
		updatePropertiesEditor(slide);
	}
	else
	{
		Slide *slide = this->slideshow->getSlide(ui->slideList->currentRow());
		SlideElement *element = slide->getElement(item->data(0, Qt::UserRole).toInt());
		if(!validateElementName(item->text(0)))
			return updateSlideTree(ui->slideList->currentRow());
		element->setValue(QStringLiteral("name"), item->text(0));
		updatePropertiesEditor(element);
	}

	setWindowModified(true);
}

void MainWindow::elementSelectionChanged()
{
	GraphicsView *view = qobject_cast<GraphicsView *>(ui->displayWidget->currentWidget());
	view->setFocus();
	view->scene()->blockSignals(true);
	view->scene()->clearSelection();
	foreach(QTreeWidgetItem *item, ui->slideTree->selectedItems())
	{
		const int index = item->data(0, Qt::UserRole).toInt();
		QGraphicsItem *graphicsItem = sceneItemFromIndex(index);
		if(graphicsItem != 0)
			graphicsItem->setSelected(true);
	}
	updateCurrentPropertiesEditor();
	updateMediaPreview();
	view->scene()->blockSignals(false);

	updateSelectionActions();
}

void MainWindow::deleteElements()
{
	const int index = ui->slideList->currentRow();
	Slide *slide = this->slideshow->getSlide(index);

	QList<int> indexesToRemove;
	foreach(QTreeWidgetItem *item, ui->slideTree->selectedItems())
		indexesToRemove.append(item->data(0, Qt::UserRole).toInt());

	qSort(indexesToRemove.begin(), indexesToRemove.end());
	QListIterator<int> iterator(indexesToRemove);
	iterator.toBack();
	while(iterator.hasPrevious())
		slide->removeElement(iterator.previous());

	updateSlide(index);
	updateSlideTree(index);
	updatePropertiesEditor(slide);
	updateMediaPreview();
	updateSelectionActions();

	setWindowModified(true);
}

void MainWindow::duplicateElements()
{
	const int index = ui->slideList->currentRow();
	Slide *slide = this->slideshow->getSlide(index);

	const int duplicatedCount = ui->slideTree->selectedItems().size();
	foreach(QTreeWidgetItem *item, ui->slideTree->selectedItems())
	{
		const int elementIndex = item->data(0, Qt::UserRole).toInt();
		SlideElement *sourceElement = slide->getElement(elementIndex);
		SlideElement *newElement = (SlideElement *)QMetaType::create(QMetaType::type(sourceElement->type()), sourceElement);
		newElement->setValue(QStringLiteral("name"), tr("Copie de %1").arg(sourceElement->getValue(QStringLiteral("name")).toString()));
		if(sceneItemFromIndex(elementIndex))
			newElement->setValue(QStringLiteral("position"), sourceElement->getValue(QStringLiteral("position")).toPoint() + QPoint(COPY_SHIFT, COPY_SHIFT));
		slide->addElement(newElement);
	}

	updateSlide(index);
	updateSlideTree(index);

	ui->slideTree->clearSelection();
	for(int i = 0; i < duplicatedCount; i++)
		ui->slideTree->topLevelItem(0)->child(i)->setSelected(true);
	elementSelectionChanged();

	setWindowModified(true);
}

void MainWindow::moveElement(const int before, const int after)
{
	const int index = ui->slideList->currentRow();
	Slide *slide = this->slideshow->getSlide(index);
	slide->moveElement(before, after);

	updateSlide(index);

	QGraphicsItem *graphicsItem = sceneItemFromIndex(after);
	if(graphicsItem != 0) graphicsItem->setSelected(true);
	updateSlideTree(index);
	elementSelectionChanged();

	setWindowModified(true);
}

void MainWindow::raiseElement()
{
	QTreeWidgetItem *item = ui->slideTree->selectedItems()[0];
	const int elementIndex = item->data(0, Qt::UserRole).toInt();
	moveElement(elementIndex, elementIndex + 1);
}

void MainWindow::lowerElement()
{
	QTreeWidgetItem *item = ui->slideTree->selectedItems()[0];
	const int elementIndex = item->data(0, Qt::UserRole).toInt();
	moveElement(elementIndex, elementIndex - 1);
}

void MainWindow::bringElementToFront()
{
	Slide *slide = this->slideshow->getSlide(ui->slideList->currentRow());
	QTreeWidgetItem *item = ui->slideTree->selectedItems()[0];
	const int elementIndex = item->data(0, Qt::UserRole).toInt();
	moveElement(elementIndex, slide->getElements().size() - 1);
}

void MainWindow::bringElementToBack()
{
	QTreeWidgetItem *item = ui->slideTree->selectedItems()[0];
	const int elementIndex = item->data(0, Qt::UserRole).toInt();
	moveElement(elementIndex, 0);
}

void MainWindow::moveSlideLeft()
{
	const int index = ui->slideList->currentRow();
	this->slideshow->moveSlide(index, index - 1);
	ui->slideList->setCurrentRow(index - 1);
	updateSlide(index); updateSlide(index - 1);
	setWindowModified(true);
	statusBar()->showMessage(tr("La diapositive a été déplacée vers la gauche."), STATUS_TIMEOUT);
}

void MainWindow::moveSlideRight()
{
	const int index = ui->slideList->currentRow();
	this->slideshow->moveSlide(index, index + 1);
	ui->slideList->setCurrentRow(index + 1);
	updateSlide(index); updateSlide(index + 1);
	setWindowModified(true);
	statusBar()->showMessage(tr("La diapositive a été déplacée vers la droite."), STATUS_TIMEOUT);
}

void MainWindow::launchViewer()
{
	if(ui->slideList->currentRow() == -1)
	{
		QMessageBox::critical(this, tr("Démarrer le diaporama"), tr("Le diaporama ne peut pas être affiché car il ne contient aucune diapositive."));
		return;
	}

	ViewWidget *viewer = new ViewWidget;
	connect(viewer, &ViewWidget::closed, this, &QMainWindow::show);
	connect(viewer, &ViewWidget::closed, ui->slideTree, &QTreeWidget::clearSelection);
	connect(viewer, &ViewWidget::closed, viewer, &QObject::deleteLater);
	viewer->setSlideshow(this->slideshow, ui->slideList->currentRow());
	viewer->showFullScreen();
	this->hide();
}

void MainWindow::copySlide()
{
	Slide *sourceSlide = this->slideshow->getSlide(ui->slideList->currentRow());
	int errorsCount = 0;
	Slide *newSlide = this->slideshow->createSlide("");
	newSlide->setValues(sourceSlide->getValues());
	newSlide->setValue(QStringLiteral("name"), tr("Copie de %1").arg(sourceSlide->getValue(QStringLiteral("name")).toString()));
	foreach(SlideElement *sourceElement, sourceSlide->getElements())
	{
		SlideElement *newElement = (SlideElement *)QMetaType::create(QMetaType::type(sourceElement->type()), sourceElement);
		if(newElement == 0)
		{
			if(errorsCount == 0)
				QMessageBox::warning(this, tr("Dupliquer la diapositive"), tr("Erreur interne lors de la copie. La diapositive affichée sera probablement incomplète. Les erreurs suivantes seront ignorées."));

			errorsCount++;
			continue;
		}

		newSlide->addElement(newElement);
	}

	displaySlide(newSlide);

	QListWidgetItem *item = ui->slideList->item(ui->slideList->count() - 1);
	ui->slideList->scrollToItem(item);
	ui->slideList->setCurrentItem(item);

	setWindowModified(true);
	statusBar()->showMessage(tr("Duplication de %0 terminée. %1 erreur(s).").arg(sourceSlide->getValue(QStringLiteral("name")).toString()).arg(errorsCount), STATUS_TIMEOUT);
}

void MainWindow::selectPrevSlide()
{
	const int index = ui->slideList->currentRow();
	if(index != 0)
		ui->slideList->setCurrentRow(index - 1);
}

void MainWindow::selectNextSlide()
{
	const int index = ui->slideList->currentRow();
	if(index != this->slideshow->getSlides().size() - 1)
		ui->slideList->setCurrentRow(index + 1);
}

void MainWindow::print()
{
	if(ui->slideList->currentRow() == -1)
	{
		QMessageBox::critical(this, ui->actionPrint->text(), tr("Le diaporama ne peut pas être imprimé car il ne contient aucune diapositive."));
		return;
	}

	statusBar()->showMessage(tr("Configuration de l'imprimante..."));

	QPrinter printer;
	printer.setDocName(this->windowTitle().remove(QRegExp("\\[\\*\\].+$")));

	QPrintDialog *dialog = new QPrintDialog(&printer, this);
	dialog->setWindowTitle(ui->actionPrint->text());
	dialog->setOption(QAbstractPrintDialog::PrintCurrentPage);
	if(dialog->exec() != QDialog::Accepted)
		return statusBar()->clearMessage();

	const int slidesCount = ui->displayWidget->count();
	int fromPage = printer.fromPage() > 0 ? printer.fromPage() - 1 : 0;
	int toPage = printer.toPage() > 0 ? printer.toPage() : slidesCount;

	if(printer.printRange() == QPrinter::CurrentPage)
	{
		fromPage = ui->slideList->currentRow();
		toPage = fromPage + 1;
	}

	statusBar()->showMessage(tr("Préparation de l'impression..."));

	QPainter painter;
	painter.begin(&printer);

	QRect pageRect = printer.pageRect();
	pageRect.setTop(25);

	for(int page = fromPage; page < toPage; page++)
	{
		statusBar()->showMessage(tr("Impression en cours de la diapositive %1...").arg(page + 1));

		painter.drawText(QRectF(0, 10, printer.pageRect().width(), 15), Qt::AlignCenter, ui->slideList->item(page)->text());

		GraphicsView *view = qobject_cast<GraphicsView *>(ui->displayWidget->widget(page));
		view->scene()->clearSelection();
		view->scene()->render(&painter, pageRect);

		if(page < slidesCount - 1)
			printer.newPage();
	}

	painter.end();

	statusBar()->showMessage(tr("Impression terminée."), STATUS_TIMEOUT);
}

void MainWindow::moveFinishTimerTimeout()
{
	updateIcon(ui->slideList->currentRow());
	updateCurrentPropertiesEditor();
}

QMenu *MainWindow::createSlideContextMenu()
{
	QMenu *menu = new QMenu(this);
	if(!ui->slideTree->selectedItems().isEmpty())
	{
		menu->addActions(selectionActions->actions());
		menu->addSeparator();
	}
	menu->addAction(ui->actionSelectAll);
	menu->addAction(ui->actionRepaint);

	QAction *separator = menu->addSeparator();
	separator->setText(ui->menuInsertElement->menuAction()->text());
	separator->setVisible(true);

	menu->addActions(insertActions);
	return menu;
}

void MainWindow::displayViewContextMenu(const QPoint &pos)
{
	GraphicsView *view = qobject_cast<GraphicsView *>(sender());
	QGraphicsItem *item = view->scene()->itemAt(view->mapToScene(pos), view->transform());

	if(item && !view->scene()->selectedItems().contains(item))
	{
		while(item->parentItem())
			item = item->parentItem();

		if(item->flags().testFlag(QGraphicsItem::ItemIsSelectable))
		{
			view->scene()->clearSelection();
			item->setSelected(true);
		}
	}

	QMenu *menu = createSlideContextMenu();
	menu->exec(view->mapToGlobal(pos));
	delete menu;
}

void MainWindow::displaySlideTreeContextMenu(const QPoint &pos)
{
	if(ui->slideList->currentRow() == -1)
		return;

	if(ui->slideTree->selectedItems().isEmpty())
	{
		QTreeWidgetItem *item = ui->slideTree->itemAt(pos);
		if(item && item->flags().testFlag(Qt::ItemIsSelectable))
		{
			ui->slideTree->clearSelection();
			item->setSelected(true);
		}
	}

	QMenu *menu = createSlideContextMenu();
	menu->exec(ui->slideTree->mapToGlobal(pos));
	delete menu;
}

void MainWindow::managePlugins()
{
	unloadPlugins();

	PluginDialog *dialog = new PluginDialog(this);
	if(dialog->exec() == QDialog::Accepted)
		QSettings().setValue(QStringLiteral("plugins"), dialog->getEnabledPlugins());

	loadPlugins();
}

void MainWindow::loadPlugins()
{
	QDir dir(PLUGINS_PATH);
	foreach(QString fileName, QSettings().value("plugins").toStringList())
	{
		QPluginLoader *loader = new QPluginLoader(dir.absoluteFilePath(fileName), this);
		Plugin* plugin = qobject_cast<Plugin *>(loader->instance());
		if(!plugin)
		{
			QMessageBox::critical(this, fileName, loader->errorString());
			continue;
		}

		plugin->setWindow(this);
		plugins << loader;

		SlideElementTypeList types = plugin->getElementTypes();
		foreach(const SlideElementType type, types)
			this->registerElementType(type);
		pluginTypes << types;
	}

	ui->actionAboutPlugins->setDisabled(plugins.isEmpty());
}

void MainWindow::unloadPlugins()
{
	while(!plugins.isEmpty())
	{
		QPluginLoader *loader = plugins.takeFirst();
		loader->unload();
		delete loader;
	}

	while(!pluginTypes.isEmpty())
		this->unregisterElementType(pluginTypes.takeFirst());
}

void MainWindow::aboutPlugins()
{
	QDir dir(PLUGINS_PATH);
	QString text;
	foreach(QPluginLoader *loader, plugins)
	{
		const QJsonObject metaData = loader->metaData().value("MetaData").toObject();
		const QString name = metaData.value("name").toString();
		const QString about = metaData.value("about").toString();
		const QString version = metaData.value("version").toString();
		const QString author = metaData.value("author").toString();

		text += QString("<p><strong>%1 %2</strong> <small>%3</small><br />Auteur : %4</p><p><i>%5</i></p>")
				.arg(name, version, dir.relativeFilePath(loader->fileName()), author, about);
	}

	QMessageBox::about(this, ui->actionAboutPlugins->text(), text);
}

void MainWindow::aboutQt()
{
	qApp->aboutQt();
}

void MainWindow::displayRecentFiles()
{
	ui->menuRecentFiles->clear();
	foreach(QString file, QSettings().value("recentFiles").toStringList())
		ui->menuRecentFiles->addAction(file);
}

void MainWindow::openRecentFile(QAction *action)
{
	openSlideshow(action->text());
}

void MainWindow::displayInsertElemMenu()
{
	ui->menuInsertElement->clear();
	ui->menuInsertElement->addActions(insertActions);
}

void MainWindow::resizeSlideshow()
{
	GeometryDialog *dialog = new GeometryDialog(slideshow->getValue(QStringLiteral("geometry")).toRect(), this);
	if(dialog->exec() == QDialog::Rejected)
		return;

	QRect newRect = dialog->getRect();
	if(newRect.isNull())
	{
		newRect = QDesktopWidget().screenGeometry();
		slideshow->unsetValue("geometry");
	}
	else
		slideshow->setValue(QStringLiteral("geometry"), newRect);

	const int slideCount = ui->displayWidget->count();

	QProgressDialog *progress = new QProgressDialog(this);
	progress->setWindowTitle(ui->actionResizeSlideshow->text());
	progress->setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
	progress->setCancelButtonText(QString());
	progress->setLabelText(tr("Mise à jour de toutes les diapositives en cours..."));
	progress->setMaximum(slideCount);
	progress->open();

	for(int index = 0; index < slideCount; index++)
	{
		progress->setValue(index + 1);

		GraphicsView *view = qobject_cast<GraphicsView *>(ui->displayWidget->widget(index));
		view->scene()->setSceneRect(newRect);
		updateSlide(index);
	}

	setWindowModified(true);
	dialog->deleteLater();

	progress->close();
	progress->deleteLater();
}

void MainWindow::registerElementType(const SlideElementType &type)
{
	QAction *action = new QAction(type.getIcon(), type.getLabel(), this);
	action->setData(type.getTypeId());
	connect(action, SIGNAL(triggered()), this, SLOT(insertElementFromAction()));

	insertActions.append(action);
}

void MainWindow::unregisterElementType(const SlideElementType &type)
{
	const int actionCount = insertActions.size();
	for(int i = 0; i < actionCount; i++)
	{
		QAction *action = insertActions[i];
		if(action->data().toInt() != type.getTypeId())
			continue;

		insertActions.takeAt(i)->deleteLater();
		break;
	}
}

void MainWindow::insertElement(SlideElement *element)
{
	const int index = ui->slideList->currentRow();
	Slide *slide = this->slideshow->getSlide(index);
	element->setValue(QStringLiteral("name"), element->getValue(QStringLiteral("name")).toString().arg(slide->getElements().size() + 1));
	slide->addElement(element);

	updateSlide(index);
	updateSlideTree(index);

	ui->slideTree->clearSelection();
	ui->slideTree->topLevelItem(0)->child(0)->setSelected(true);
	elementSelectionChanged();

	setWindowModified(true);
}

void MainWindow::insertElementFromAction()
{
	QAction *action = qobject_cast<QAction *>(sender());

	SlideElement *element = (SlideElement *)QMetaType::create(action->data().toInt());
	element->setValue(QStringLiteral("name"), tr("%1 %2").arg(action->text()));

	insertElement(element);
}

void MainWindow::displaySlideListContextMenu(const QPoint &pos)
{
	QListWidgetItem *item = ui->slideList->itemAt(pos);

	QMenu *menu = new QMenu(this);
	menu->addAction(ui->actionAddSlide);

	if(item)
	{
		menu->addSeparator();
		menu->addAction(ui->actionRenameSlide);
		menu->addAction(ui->actionDuplicateSlide);
		menu->addAction(ui->actionMoveSlideLeft);
		menu->addAction(ui->actionMoveSlideRight);
		menu->addAction(ui->actionDeleteSlide);
	}

	menu->exec(ui->slideList->mapToGlobal(pos));
	delete menu;
}

void MainWindow::playPreview()
{
	previewPlayer->play();
}

void MainWindow::pausePreview()
{
	previewPlayer->pause();
}

void MainWindow::setPreviewVolume(int newVolume)
{
	previewPlayer->setVolume(newVolume);
	QSettings().setValue(QStringLiteral("previewVolume"), newVolume);
}

void MainWindow::setPreviewPosition(int newPos)
{
	previewPlayer->setPosition(newPos+1);
}

void MainWindow::previewSeekableChanged(bool seekable)
{
	ui->seekSlider->setEnabled(seekable);
	ui->volumeSlider->setEnabled(seekable);
}

void MainWindow::previewDurationChanged(qint64 duration)
{
	ui->seekSlider->setMaximum(duration);
	previewPositionChanged(0);

	ui->durationLabel->setText(msToString(duration));
}

void MainWindow::previewPositionChanged(qint64 position)
{
	ui->seekSlider->blockSignals(true);
	ui->seekSlider->setValue(position);
	ui->seekSlider->blockSignals(false);

	ui->positionLabel->setText(msToString(position + 1));
}

void MainWindow::previewStateChanged(QMediaPlayer::State state)
{
	ui->pauseButton->setVisible(state == QMediaPlayer::PlayingState);
	ui->playButton->setVisible(state != QMediaPlayer::PlayingState);
}

QString MainWindow::msToString(const int ms) const
{
	const int hours = ms / (1000 * 60 * 60);
	const int minutes = (ms % (1000*60*60)) / (1000*60);
	const int secs = ((ms % (1000*60*60)) % (1000*60)) / 1000;

	if(hours > 0)
		return tr("%0h%1:%2").arg(
			QString::number(hours).rightJustified(2, '0', true),
			QString::number(minutes).rightJustified(2, '0', true),
			QString::number(secs).rightJustified(2, '0', true)
		);
	else
		return tr("%0:%1").arg(
			QString::number(minutes).rightJustified(2, '0', true),
			QString::number(secs).rightJustified(2, '0', true)
		);
}

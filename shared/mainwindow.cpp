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

	qRegisterMetaType<AudioElement>("AudioElement");
	qRegisterMetaType<EllipseElement>("EllipseElement");
	qRegisterMetaType<ImageElement>("ImageElement");
	qRegisterMetaType<LineElement>("LineElement");
	qRegisterMetaType<MovieElement>("MovieElement");
	qRegisterMetaType<RectElement>("RectElement");
	qRegisterMetaType<TextElement>("TextElement");

	new QShortcut(QKeySequence("Shift+Tab"), this, SLOT(selectNextSlide()));
	new QShortcut(QKeySequence("Ctrl+Shift+Tab"), this, SLOT(selectPrevSlide()));

	this->restoreGeometry(QSettings().value("mainWindow/geometry").toByteArray());
	this->restoreState(QSettings().value("mainWindow/state").toByteArray());

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
	connect(ui->menuRecentFiles, SIGNAL(aboutToShow()), this, SLOT(displayRecentFiles()));
	connect(ui->menuRecentFiles, SIGNAL(triggered(QAction*)), this, SLOT(openRecentFile(QAction*)));

	ui->actionCurrentSlide->setSeparator(true);

	connect(ui->menuInsertElement, SIGNAL(aboutToShow()), this, SLOT(displayInsertElemMenu()));

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

	ui->seekSlider->setMediaObject(ui->videoPlayer->mediaObject());
	ui->volumeSlider->setAudioOutput(ui->videoPlayer->audioOutput());

	this->slideshow = 0;
	this->newSlideshowCount = 0;
	this->commandLineHelp = commandLineHelp;

	if(openFile.isEmpty() || !openSlideshow(openFile))
	{
		newSlideshow();
		statusBar()->showMessage(tr("Merci d'utiliser cfiSlides !"), STATUS_TIMEOUT);
	}

	moveFinishTimer.setInterval(REFRESH_INTERVAL);
	moveFinishTimer.setSingleShot(true);
	connect(&moveFinishTimer, SIGNAL(timeout()), this, SLOT(moveFinishTimerTimeout()));

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

void MainWindow::setInsertElemMenu(QMenu *addElemMenu)
{
	QAction *separator = addElemMenu->addSeparator();
	separator->setText(ui->menuInsertElement->menuAction()->text());
	separator->setVisible(true);

	QAction *actionRect = new QAction(QIcon::fromTheme("insert-rectangle"), tr("Rectangle"), this);
	connect(actionRect, SIGNAL(triggered()), this, SLOT(addRectElement()));
	addElemMenu->addAction(actionRect);

	QAction *actionEllipse = new QAction(QIcon::fromTheme("insert-ellipse"), tr("Ellipse"), this);
	connect(actionEllipse, SIGNAL(triggered()), this, SLOT(addEllipseElement()));
	addElemMenu->addAction(actionEllipse);

	QAction *actionLine = new QAction(QIcon::fromTheme("insert-horizontal-rule"), tr("Ligne"), this);
	connect(actionLine, SIGNAL(triggered()), this, SLOT(addLineElement()));
	addElemMenu->addAction(actionLine);

	QAction *actionText = new QAction(QIcon::fromTheme("insert-text"), tr("Texte"), this);
	connect(actionText, SIGNAL(triggered()), this, SLOT(addTextElement()));
	addElemMenu->addAction(actionText);

	QAction *actionPixmap = new QAction(QIcon::fromTheme("insert-image"), tr("Image"), this);
	connect(actionPixmap, SIGNAL(triggered()), this, SLOT(addImageElement()));
	addElemMenu->addAction(actionPixmap);

	QAction *actionMovie = new QAction(QIcon::fromTheme("insert-video"), tr("Vidéo"), this);
	connect(actionMovie, SIGNAL(triggered()), this, SLOT(addMovieElement()));
	addElemMenu->addAction(actionMovie);

	QAction *actionAudio = new QAction(QIcon::fromTheme("insert-audio"), tr("Son"), this);
	connect(actionAudio, SIGNAL(triggered()), this, SLOT(addAudioElement()));
	addElemMenu->addAction(actionAudio);

	emit insertElemMenu(addElemMenu);
}

void MainWindow::setWindowModified(const bool modified)
{
	QMainWindow::setWindowModified(modified);
	ui->actionSave->setEnabled(modified);
}

void MainWindow::setWindowTitle(const QString &fileName)
{
	QMainWindow::setWindowTitle(tr("%1[*] - cfiSlides").arg(fileName));
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

	QtLockedFile file(newFile);
	if(!file.open(QIODevice::ReadOnly))
	{
		QMessageBox::critical(this, ui->actionOpen->text(), tr("Impossible de lire le contenu du fichier."));
		return false;
	}
	file.lock(QtLockedFile::ReadLock);

	int errorsCount = 0;

	QDataStream in(&file);
	QString fileAppName;
	in >> fileAppName;
	if(fileAppName != qApp->applicationName())
	{
		QMessageBox::critical(this, ui->actionOpen->text(), tr("Le fichier demandé ne peut pas être ouvert avec cfiSlides."));
		return false;
	}
	int slidesCount = 0;
	in >> slidesCount;

	if(this->slideshow != 0)
	{
		if(!closeSlideshow())
			return false;
	}

	this->slideshow = new Slideshow();
	this->currentSlideActions->setEnabled(false);

	QProgressDialog *progress = new QProgressDialog(this);
	progress->setWindowTitle(ui->actionOpen->text());
	progress->setMinimumDuration(100);
	progress->setCancelButtonText(QString());
	progress->setLabelText(tr("Chargement et affichage du diaporama en cours..."));
	progress->setMaximum(slidesCount);
	progress->open();

	for(int si = 0; si < slidesCount; si++)
	{
		progress->setValue(si + 1);

		QMap<QString, QVariant> properties;
		in >> properties;

		Slide *slide = this->slideshow->createSlide(QString::number(si));
		slide->setProperties(properties);

		int elementsCount = 0;
		in >> elementsCount;
		for(int ei = 0; ei < elementsCount; ei++)
		{
			char *type;
			in >> type;

			QMap<QString, QVariant> properties;
			in >> properties;

			SlideElement *element = (SlideElement *)QMetaType::construct(QMetaType::type(type));
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

			element->setProperties(properties);
			slide->addElement(element);
		}

		displaySlide(slide);
	}

	file.unlock();
	file.close();

	QString fileName = QFileInfo(newFile).fileName();
	this->setWindowTitle(fileName);
	this->setWindowFilePath(newFile);

	QStringList recentFiles = QSettings().value("recentFiles").toStringList();
	recentFiles.insert(0, newFile);
	recentFiles.removeDuplicates();
	if(recentFiles.count() > RECENT_FILES_MAX)
		recentFiles.removeLast();

	QSettings().setValue("recentFiles", recentFiles);

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

	QtLockedFile file(this->windowFilePath());
	if(!file.open(QIODevice::WriteOnly))
	{
		QMessageBox::critical(this, ui->actionSave->text(), tr("Impossible d'écrire dans le fichier."));
		return false;
	}
	file.lock(QtLockedFile::WriteLock);

	QDataStream out(&file);
	out.setVersion(QDataStream::Qt_4_8);

	QList<Slide *> slides = slideshow->getSlides();
	out << QString(qApp->applicationName());
	out << slides.size();
	foreach(Slide *slide, slides)
	{
		out << slide->getProperties();
		QList<SlideElement *> elements = slide->getElements();
		out << elements.size();
		foreach(SlideElement *element, elements)
		{
			out << element->type();
			out << element->getProperties();
		}
	}

	file.unlock();
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
		const int choice = QMessageBox::warning(this, tr("Sauvegarder les changements"), tr("Voulez-vous enregistrer les changements apportés à ce diaporama avant de le fermer ?"), QMessageBox::Save, QMessageBox::Discard, QMessageBox::Cancel);
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

	QMainWindow::setWindowTitle("[*]cfiSlides");
	this->setWindowModified(false);
	delete this->slideshow;

	ui->slideList->clear();
	ui->slideTree->clear();
	clearPropertiesEditor();
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

	QSettings().setValue("mainWindow/geometry", this->saveGeometry());
	QSettings().setValue("mainWindow/state", this->saveState());

	QMainWindow::closeEvent(event);
}

void MainWindow::createEmptySlide()
{
	Slide *slide = this->slideshow->createSlide(tr("Diapositive %1").arg(this->slideshow->getSlides().size() + 1));
	displaySlide(slide);

	QListWidgetItem *item = ui->slideList->item(ui->slideList->count() - 1);
	ui->slideList->scrollToItem(item);
	ui->slideList->setCurrentItem(item);

	setWindowModified(true);
	statusBar()->showMessage(tr("Diapositive créée."), STATUS_TIMEOUT);
}

void MainWindow::displaySlide(Slide *slide)
{
	statusBar()->showMessage(tr("Affichage de %1...").arg(slide->getValue("name").toString()));

	QGraphicsScene *scene = new QGraphicsScene();
	scene->setSceneRect(QDesktopWidget().screenGeometry());
	scene->setItemIndexMethod(QGraphicsScene::NoIndex);
	connect(scene, SIGNAL(selectionChanged()), this, SLOT(updateCurrentSlideTree()));
	connect(scene, SIGNAL(selectionChanged()), this, SLOT(updateSelectionActions()));
	connect(scene, SIGNAL(selectionChanged()), this, SLOT(updateCurrentPropertiesEditor()));
	connect(scene, SIGNAL(selectionChanged()), this, SLOT(updateMediaPreview()));
	slide->render(scene, true);

	GraphicsView *view = new GraphicsView(scene);
	view->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(view, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(showViewContextMenu(const QPoint &)));
	ui->displayWidget->addWidget(view);

	QPixmap pixmap(scene->sceneRect().size().toSize());
	QPainter painter(&pixmap);
	painter.setRenderHint(QPainter::Antialiasing);
	scene->render(&painter, scene->sceneRect());

	QIcon icon(pixmap.scaledToWidth(ui->slideList->iconSize().width()));
	QListWidgetItem *newItem = new QListWidgetItem(icon, slide->getValue("name").toString());
	newItem->setFlags(newItem->flags() ^ Qt::ItemIsEditable);
	ui->slideList->addItem(newItem);

	this->currentSlideActions->setEnabled(true);

	connect(slide, SIGNAL(modified()), this, SLOT(slideModified()));
	connect(slide, SIGNAL(moved()), this, SLOT(slideElementMoved()));
	connect(slide, SIGNAL(refresh()), this, SLOT(reRenderSlide()));

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
	ui->slideList->item(index)->setText(slide->getValue("name").toString());
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
	topLevel->setText(0, slide->getValue("name").toString());
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
		treeItem->setText(0, element->getValue("name").toString());
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

void MainWindow::updatePropertiesEditor(const BaseElement *element)
{
	clearPropertiesEditor();
	element->bindProperties(ui->propertiesEditor);
}

void MainWindow::clearPropertiesEditor()
{
	foreach(QtProperty *prop, ui->propertiesEditor->properties())
	{
		ui->propertiesEditor->unsetFactoryForManager(prop->propertyManager());
		prop->propertyManager()->deleteLater();
		delete prop;
	}

	ui->propertiesEditor->clear();
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

	Slide *slide = this->slideshow->getSlide(ui->slideList->currentRow());

	const int selectedItemsCount = ui->slideTree->selectedItems().size();
	const bool enableUpDown = slide->getElements().size() > 1 && selectedItemsCount == 1;
	ui->actionRaiseElement->setEnabled(enableUpDown && ui->slideTree->selectedItems()[0] != ui->slideTree->topLevelItem(0)->child(0));
	ui->actionLowerElement->setEnabled(enableUpDown && selectedItemsCount > 0 && ui->slideTree->selectedItems()[selectedItemsCount - 1] != ui->slideTree->topLevelItem(0)->child(ui->slideTree->topLevelItem(0)->childCount() - 1));
}

void MainWindow::updateMediaPreview()
{
	ui->videoPlayer->stop();
	ui->videoPlayer->update();
	ui->mediaPreview->setEnabled(false);

	if(ui->slideTree->selectedItems().size() < 1 || !ui->mediaDock->isVisible())
		return;

	Slide *slide = this->slideshow->getSlide(ui->slideList->currentRow());
	QTreeWidgetItem *item = ui->slideTree->selectedItems()[0];
	SlideElement *element = slide->getElement(item->data(0, Qt::UserRole).toInt());
	QString previewUrl = element->previewUrl();

	if(!previewUrl.isEmpty())
	{
		ui->videoPlayer->play(previewUrl);
		ui->mediaPreview->setEnabled(true);
	}
}

void MainWindow::renameSlide()
{
	const int index = ui->slideList->currentRow();
	Slide *slide = this->slideshow->getSlide(index);
	bool ok;
	QString newName = QInputDialog::getText(this, ui->actionRenameSlide->text(), tr("Nouveau nom pour cette diapositive :"), QLineEdit::Normal, slide->getValue("name").toString(), &ok);
	if(!ok || !validateSlideName(newName)) return;

	slide->setValue("name", newName);
	updateSlide(index);
	updateSlideTree(index);
	updateCurrentPropertiesEditor();
	setWindowModified(true);
}

bool MainWindow::validateSlideName(const QString &name)
{
	if(name.trimmed().isEmpty())
	{
		QMessageBox::critical(qApp->activeWindow(), tr("Renommer la diapositive"), tr("Le nom de la diapositive ne peut pas être vide."));
		return false;
	}

	return true;
}

bool MainWindow::validateElementName(const QString &name)
{
	if(name.trimmed().isEmpty())
	{
		QMessageBox::critical(qApp->activeWindow(), tr("Renommer l'élément"), tr("Le nom de l'élément ne peut pas être vide."));
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
		if(item != 0)
			item->setSelected(true);
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
	QMessageBox::about(this, tr("À propos de %1 - %2").arg(qApp->applicationName()).arg(qApp->applicationPid()),
					   QString("<p><strong>%1 %2</strong><br /><kbd>%3</kbd></p>")
					   .arg(qApp->applicationName(), qApp->applicationVersion(), commandLineHelp) +

					   "<p>Copyright (C) 2013  Christian Fillion</p>" +

					   "<p>cfiSlides is free software: you can redistribute it and/or modify" +
					   " it under the terms of the GNU General Public License as published by" +
					   " the Free Software Foundation, either version 3 of the License, or" +
					   " (at your option) any later version.</p>" +

					   "<p>cfiSlides is distributed in the hope that it will be useful," +
					   " but WITHOUT ANY WARRANTY; without even the implied warranty of" +
					   " MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the" +
					   " GNU General Public License for more details.</p>" +

					   "<p>You should have received a copy of the GNU General Public License along with cfiSlides." +
					   " If not, see <a href=\"http://www.gnu.org/licenses/\">http://www.gnu.org/licenses/</a>.</p>");
}

void MainWindow::currentSlideChanged(int currentRow)
{
	if(currentRow == -1)
		return;

	ui->displayWidget->setCurrentIndex(currentRow);
	ui->actionMoveSlideLeft->setEnabled(this->slideshow->getSlides().size() > 0 && currentRow != 0);
	ui->actionMoveSlideRight->setEnabled(this->slideshow->getSlides().size() > 0 && currentRow != this->slideshow->getSlides().size() - 1);

	updateSlideTree(currentRow);
	updateCurrentPropertiesEditor();
	updateMediaPreview();
	updateSelectionActions();
}

void MainWindow::slideItemChanged(QListWidgetItem *item)
{
	const int index = ui->slideList->row(item);
	Slide *slide = this->slideshow->getSlide(index);
	if(!validateSlideName(item->text()))
	{
		ui->slideList->blockSignals(true);
		ui->slideList->item(index)->setText(slide->getValue("name").toString());
		ui->slideList->blockSignals(false);
		return;
	}

	slide->setValue("name", item->text());
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

		slide->setValue("name", item->text(0));
		ui->slideList->item(index)->setText(item->text(0));
		updatePropertiesEditor(slide);
	}
	else
	{
		Slide *slide = this->slideshow->getSlide(ui->slideList->currentRow());
		SlideElement *element = slide->getElement(item->data(0, Qt::UserRole).toInt());
		if(!validateElementName(item->text(0)))
			return updateSlideTree(ui->slideList->currentRow());
		element->setValue("name", item->text(0));
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
		SlideElement *sourceElement = slide->getElement(item->data(0, Qt::UserRole).toInt());
		SlideElement *newElement = (SlideElement *)QMetaType::construct(QMetaType::type(sourceElement->type()), sourceElement);
		newElement->setValue("name", tr("Copie de %1").arg(sourceElement->getValue("name").toString()));
		if(sourceElement->getProperties().contains("position"))
			newElement->setValue("position", sourceElement->getValue("position").toPoint() + QPoint(COPY_SHIFT, COPY_SHIFT));
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

void MainWindow::raiseElement()
{
	const int index = ui->slideList->currentRow();
	Slide *slide = this->slideshow->getSlide(index);
	QTreeWidgetItem *item = ui->slideTree->selectedItems()[0];
	const int elementIndex = item->data(0, Qt::UserRole).toInt();
	slide->moveElement(elementIndex, elementIndex + 1);

	updateSlide(index);

	QGraphicsItem *graphicsItem = sceneItemFromIndex(elementIndex + 1);
	if(graphicsItem != 0) graphicsItem->setSelected(true);
	updateSlideTree(index);
	elementSelectionChanged();

	setWindowModified(true);
}

void MainWindow::lowerElement()
{
	const int index = ui->slideList->currentRow();
	Slide *slide = this->slideshow->getSlide(index);
	QTreeWidgetItem *item = ui->slideTree->selectedItems()[0];
	const int elementIndex = item->data(0, Qt::UserRole).toInt();
	slide->moveElement(elementIndex, elementIndex - 1);

	updateSlide(index);

	QGraphicsItem *graphicsItem = sceneItemFromIndex(elementIndex - 1);
	if(graphicsItem != 0) graphicsItem->setSelected(true);
	updateSlideTree(index);
	elementSelectionChanged();

	setWindowModified(true);
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

void MainWindow::addElement(SlideElement *element)
{
	const int index = ui->slideList->currentRow();
	Slide *slide = this->slideshow->getSlide(index);
	element->setValue("name", element->getValue("name").toString().arg(slide->getElements().size() + 1));
	slide->addElement(element);

	updateSlide(index);
	updateSlideTree(index);

	ui->slideTree->clearSelection();
	ui->slideTree->topLevelItem(0)->child(0)->setSelected(true);
	elementSelectionChanged();

	setWindowModified(true);
}

void MainWindow::addImageElement()
{
	QString image = QFileDialog::getOpenFileName(this, tr("Ajouter une image"), QFileInfo(this->windowFilePath()).absolutePath(), IMAGE_FILTER);
	if(image.isEmpty())
		return;

	ImageElement *element = new ImageElement;
	element->setValue("name", tr("Image %1"));
	element->setValue("src", image);
	element->setValue("size", QPixmap(image).size());

	addElement(element);
}

void MainWindow::addRectElement()
{
	RectElement *element = new RectElement;
	element->setValue("name", tr("Rectangle %1"));
	element->setValue("size", QSize(DEFAULT_SIZE, DEFAULT_SIZE));

	addElement(element);
}

void MainWindow::addEllipseElement()
{
	EllipseElement *element = new EllipseElement;
	element->setValue("name", tr("Ellipse %1"));
	element->setValue("size", QSize(DEFAULT_SIZE, DEFAULT_SIZE));

	addElement(element);
}

void MainWindow::addTextElement()
{
	TextInputDialog *dialog = new TextInputDialog(this);
	dialog->setWindowTitle(tr("Ajouter du texte"));
	if(dialog->exec() == QDialog::Rejected)
		return;

	TextElement *element = new TextElement;
	element->setValue("name", tr("Texte %1"));
	element->setValue("width", DEFAULT_TEXT_WIDTH);
	element->setValue("text", dialog->text());

	addElement(element);
}

void MainWindow::addMovieElement()
{
	QString movie = QFileDialog::getOpenFileName(this, tr("Ajouter une vidéo"), QFileInfo(this->windowFilePath()).absolutePath(), MOVIE_FILTER);
	if(movie.isEmpty())
		return;

	GraphicsView *view = qobject_cast<GraphicsView *>(ui->displayWidget->currentWidget());

	MovieElement *element = new MovieElement;
	element->setValue("name", tr("Vidéo %1"));
	element->setValue("src", movie);
	element->setValue("size", view->scene()->sceneRect().size().toSize() / DEFAULT_SIZE_SCALE); // TODO: use movie size

	addElement(element);
}

void MainWindow::addAudioElement()
{
	QString soundPath = QFileDialog::getOpenFileName(this, tr("Ajouter un son"), QFileInfo(this->windowFilePath()).absolutePath(), AUDIO_FILTER);
	if(soundPath.isEmpty())
		return;

	AudioElement *element = new AudioElement;
	element->setValue("name", tr("Son %1"));
	element->setValue("src", soundPath);

	addElement(element);
}

void MainWindow::addLineElement()
{
	LineElement *element = new LineElement;
	element->setValue("name", tr("Ligne %1"));
	element->setValue("size", DEFAULT_LINE_THICKNESS);
	element->setValue("stop", QPoint(DEFAULT_SIZE, 0));

	addElement(element);
}

void MainWindow::launchViewer()
{
	if(this->slideshow->getSlides().isEmpty())
	{
		QMessageBox::critical(this, tr("Démarrer le diaporama"), tr("Le diaporama ne peut pas être affiché car il ne contient aucune diapositive."));
		return;
	}

	ViewWidget *viewer = new ViewWidget();
	connect(viewer, SIGNAL(closed(int)), this, SLOT(show()));
	connect(viewer, SIGNAL(closed(int)), ui->slideTree, SLOT(clearSelection()));
	connect(viewer, SIGNAL(closed(int)), viewer, SLOT(deleteLater()));
	viewer->setSlideshow(this->slideshow, ui->slideList->currentRow());
	viewer->showFullScreen();
	this->hide();
}

void MainWindow::copySlide()
{
	Slide *sourceSlide = this->slideshow->getSlide(ui->slideList->currentRow());
	int errorsCount = 0;
	Slide *newSlide = this->slideshow->createSlide("");
	newSlide->setProperties(sourceSlide->getProperties());
	newSlide->setValue("name", tr("Copie de %1").arg(sourceSlide->getValue("name").toString()));
	foreach(SlideElement *sourceElement, sourceSlide->getElements())
	{
		SlideElement *newElement = (SlideElement *)QMetaType::construct(QMetaType::type(sourceElement->type()), sourceElement);
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
	statusBar()->showMessage(tr("Duplication de %0 terminée. %1 erreur(s).").arg(sourceSlide->getValue("name").toString()).arg(errorsCount), STATUS_TIMEOUT);
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
	statusBar()->showMessage(tr("Configuration de l'imprimante..."));

	QPrinter printer;
	printer.setDocName(this->windowTitle().remove(QRegExp("\\[\\*\\].+$")));

	QPrintDialog *dialog = new QPrintDialog(&printer, this);
	dialog->setWindowTitle(ui->actionPrint->text());
	dialog->setOption(QAbstractPrintDialog::PrintCurrentPage);
	if(dialog->exec() != QDialog::Accepted)
		return;

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

void MainWindow::showViewContextMenu(const QPoint &pos)
{
	GraphicsView *view = qobject_cast<GraphicsView *>(sender());
	QPoint globalPos = view->mapToGlobal(pos);
	QMenu *menu = new QMenu(this);
	menu->addAction(ui->actionSelectAll);
	menu->addAction(ui->actionRepaint);
	setInsertElemMenu(menu);
	menu->exec(globalPos);
	delete menu;
}

void MainWindow::managePlugins()
{
	unloadPlugins();

	PluginDialog *dialog = new PluginDialog(this);
	if(dialog->exec() == QDialog::Accepted)
		QSettings().setValue("plugins", dialog->getEnabledPlugins());

	loadPlugins();
}

void MainWindow::loadPlugins()
{
	QDir dir(PLUGINS_PATH);
	foreach(QString fileName, QSettings().value("plugins").toStringList())
	{
		qDebug() << "Loading plugin" << fileName;
		QPluginLoader *loader = new QPluginLoader(dir.absoluteFilePath(fileName), this);
		Plugin* plugin = qobject_cast<Plugin *>(loader->instance());
		if(!plugin)
		{
			QMessageBox::critical(this, fileName, loader->errorString());
			continue;
		}

		plugin->setWindow(this);
		plugins.append(loader);
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
}

void MainWindow::aboutPlugins()
{
	QDir dir(PLUGINS_PATH);
	QString text;
	foreach(QPluginLoader *loader, plugins)
	{
		Plugin *plugin = qobject_cast<Plugin *>(loader->instance());
		text += QString("<p><strong>%1 %2</strong> <small>%3</small><br />Auteur : %4</p>")
				.arg(plugin->name(), plugin->version(), dir.relativeFilePath(loader->fileName()), plugin->author()) +
				"<p><i>" + plugin->about() + "</i></p>";
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
	setInsertElemMenu(ui->menuInsertElement);
}

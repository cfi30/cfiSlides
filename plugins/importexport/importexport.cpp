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

#include <QMessageBox>
#include <QMenuBar>
#include <QMenu>
#include <QStackedWidget>
#include <QStatusBar>
#include <QDesktopServices>
#include <QUrl>
#include <QProgressDialog>
#include <QListWidget>
#include <QGraphicsView>

#include "slideshow.h"
#include "slide.h"
#include "slideelement.h"
#include "configuration.h"

#include "importexport.h"
#include "exportdialog.h"
#include "importdialog.h"

ImportExport::ImportExport()
{
	importAction = new QAction(QIcon::fromTheme("document-import"), tr("Importer"), this);
	importAction->setShortcut(QKeySequence("Ctrl+I"));
	connect(importAction, &QAction::triggered, this, &ImportExport::launchImport);

	exportAction = new QAction(QIcon::fromTheme("document-export"), tr("Exporter"), this);
	exportAction->setShortcut(QKeySequence("Ctrl+E"));
	connect(exportAction, &QAction::triggered, this, &ImportExport::launchExport);
}

void ImportExport::initialize()
{
	QMenu *menuSlideshow = window->findChild<QMenu *>("menuSlideshow");
	QAction *actionCurrentSlide = window->findChild<QAction *>("actionCurrentSlide");

	menuSlideshow->insertAction(actionCurrentSlide, importAction);
	menuSlideshow->insertAction(actionCurrentSlide, exportAction);
}

ImportExport::~ImportExport()
{
	delete importAction;
	delete exportAction;
}

void ImportExport::launchImport()
{
	Slideshow *slideshow;
	QMetaObject::invokeMethod(window, "getSlideshow", Qt::DirectConnection, Q_RETURN_ARG(Slideshow *, slideshow));

	ImportDialog *dialog = new ImportDialog(slideshow->getSlides().size(), slideshow, window);
	connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));

	if(dialog->exec() == QDialog::Rejected)
		return;

	QList<Slide *> slides = dialog->slides();
	const int slideCount = slides.size();

	QProgressDialog *progress = new QProgressDialog(window);
	progress->setWindowTitle(dialog->windowTitle());
	progress->setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
	progress->setCancelButtonText(QString());
	progress->setLabelText(tr("Importation et affichage des éléments en cours..."));
	progress->setMaximum(slideCount);
	progress->open();

	for(int index = 0; index < slideCount; index++)
	{
		progress->setValue(index);

		Slide *tempSlide = slides[index];
		Slide *slide = slideshow->createSlide();
		slide->setValues(tempSlide->getValues());

		foreach(SlideElement *element, tempSlide->getElements())
			slide->addElement(element->clone());

		QMetaObject::invokeMethod(window, "displaySlide", Qt::DirectConnection, Q_ARG(Slide *, slide));
		QMetaObject::invokeMethod(window, "setWindowModified", Qt::DirectConnection, Q_ARG(bool, true));
	}

	progress->close();
	progress->deleteLater();
}

void ImportExport::launchExport()
{
	Slideshow *slideshow;
	QMetaObject::invokeMethod(window, "getSlideshow", Qt::DirectConnection, Q_RETURN_ARG(Slideshow *, slideshow));

	const int slideCount = slideshow->getSlides().size();
	if(slideCount < 1)
		return (void)QMessageBox::critical(window, exportAction->text(), tr("Impossible d'exporter un diaporama vide."));

	ExportDialog *dialog = new ExportDialog(window);
	dialog->setSlideCount(slideCount);
	connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));

	if(dialog->exec() == QDialog::Rejected)
		return;

	const QString format = dialog->format();
	const QString fileTemplate = dialog->fileTemplate();
	const int quality = dialog->quality();
	const QString directory = dialog->directory();
	const ExportDialog::SelectionMode selectionMode = dialog->selectionMode();
	int from = dialog->from();
	int to = dialog->to();

	if(selectionMode == ExportDialog::CurrentSlide)
		from = to = window->findChild<QListWidget *>("slideList")->currentRow();

	QProgressDialog *progress = new QProgressDialog(window);
	progress->setWindowTitle(dialog->windowTitle());
	progress->setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
	progress->setCancelButtonText(QString());
	progress->setLabelText(tr("Exportation des diapositives sélectionnées en cours..."));
	progress->setMinimum(from);
	progress->setMaximum(to);
	progress->open();

	for(int index = from; index <= to; index++)
	{
		progress->setValue(index + 1);

		QGraphicsScene *scene = new QGraphicsScene;
		scene->setSceneRect(QRect(QPoint(), slideshow->getValue(QStringLiteral("size")).toSize()));
		scene->setItemIndexMethod(QGraphicsScene::NoIndex);

		QPixmap pixmap(scene->sceneRect().size().toSize());
		QPainter painter(&pixmap);
		painter.setRenderHint(QPainter::Antialiasing);

		Slide *slide = slideshow->getSlide(index);
		slide->render(scene, false);
		scene->render(&painter, scene->sceneRect());

		delete scene;

		QString fileName = fileTemplate;
		fileName.replace("%n", slide->getValue(QStringLiteral("name")).toString());
		fileName.replace("%i", QString::number(index + 1));
		fileName.replace("%s", QFileInfo(window->windowFilePath()).baseName());
		fileName.replace("%f", format);

		if(!pixmap.save(directory + "/" + fileName, format.toLocal8Bit().data(), quality))
		{
			QMessageBox::critical(window, dialog->windowTitle(), tr("Une erreur s'est produite lors de l'enregistrement de %1.").arg(fileName), QMessageBox::Abort);
			progress->close();
			window->statusBar()->showMessage(tr("Exportation arrêtée."), STATUS_TIMEOUT);
			return;
		}
	}

	progress->close();
	progress->deleteLater();

	QSettings().setValue(QStringLiteral("exportDialog/format"), format);
	QSettings().setValue(QStringLiteral("exportDialog/quality"), quality);
	QSettings().setValue(QStringLiteral("exportDialog/template"), fileTemplate);

	QDesktopServices::openUrl(QUrl::fromLocalFile(directory));
	window->statusBar()->showMessage(tr("Exportation terminée."), STATUS_TIMEOUT);
}

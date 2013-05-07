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

#include "importexport.h"

ImportExport::ImportExport()
{
	importAction = new QAction(QIcon::fromTheme("document-import"), tr("Importer"), this);
	importAction->setShortcut(QKeySequence("Ctrl+I"));
	connect(importAction, SIGNAL(triggered()), this, SLOT(launchImport()));

	exportAction = new QAction(QIcon::fromTheme("document-export"), tr("Exporter"), this);
	exportAction->setShortcut(QKeySequence("Ctrl+E"));
	connect(exportAction, SIGNAL(triggered()), this, SLOT(launchExport()));
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

QString ImportExport::name() const
{
	return tr("Importer / Exporter");
}

QString ImportExport::version() const
{
	return "1.0";
}

QString ImportExport::author() const
{
	return "<a href=\"http://mysite.com\">Christian Fillion</a>";
}

QString ImportExport::about() const
{
	return "Demo plugin for cfiSlides.";
}

void ImportExport::launchImport()
{
	ImportDialog *dialog = new ImportDialog(window->getSlideshow()->getSlides().count(), window);
	connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));

	if(dialog->exec() == QDialog::Rejected)
		return;

	foreach(Slide *tempSlide, dialog->slides())
	{
		Slide *slide = window->getSlideshow()->createSlide("temp-name");
		slide->setProperties(tempSlide->getProperties());

		foreach(SlideElement *element, tempSlide->getElements())
			slide->addElement(element);

		window->displaySlide(slide);
		window->setWindowModified(true);
	}
}

void ImportExport::launchExport()
{
	ExportDialog *dialog = new ExportDialog(window);
	dialog->setSlideCount(window->getSlideshow()->getSlides().size());
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
	progress->setMinimumDuration(100);
	progress->setCancelButtonText(QString());
	progress->setLabelText(tr("Exportation des diapositives sélectionnées en cours..."));
	progress->setMinimum(from);
	progress->setMaximum(to);
	progress->open();

	for(int index = from; index <= to; index++)
	{
		progress->setValue(index + 1);

		Slide *slide = window->getSlideshow()->getSlide(index);
		GraphicsView *view = qobject_cast<GraphicsView *>(window->findChild<QStackedWidget *>("displayWidget")->widget(index));

		QPixmap pixmap(view->scene()->sceneRect().size().toSize());
		QPainter painter(&pixmap);
		painter.setRenderHint(QPainter::Antialiasing);
		view->scene()->clearSelection();
		view->scene()->render(&painter, view->scene()->sceneRect());

		QString fileName = fileTemplate;
		fileName.replace("%n", slide->getValue("name").toString());
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
	QSettings().setValue("exportDialog/format", format);
	QSettings().setValue("exportDialog/quality", quality);
	QSettings().setValue("exportDialog/template", fileTemplate);
	window->statusBar()->showMessage(tr("Exportation terminée."), STATUS_TIMEOUT);

	QDesktopServices::openUrl(QUrl::fromLocalFile(directory));
}

Q_EXPORT_PLUGIN2(import_export, ImportExport)

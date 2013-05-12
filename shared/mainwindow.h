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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QFileDialog>
#include <QCloseEvent>
#include <QFile>
#include <QSettings>
#include <QPainter>
#include <QDesktopWidget>
#include <QGraphicsScene>
#include <QPixmap>
#include <QListWidgetItem>
#include <QTreeWidgetItem>
#include <QInputDialog>
#include <QtTreePropertyBrowser>
#include <QShortcut>
#include <QPrinter>
#include <QPrintDialog>
#include <QTimer>
#include <QPluginLoader>
#include <QActionGroup>

#include "graphicsview.h"
#include "slideshow.h"
#include "slide.h"
#include "slideelement.h"
#include "imageelement.h"
#include "rectelement.h"
#include "ellipseelement.h"
#include "textelement.h"
#include "textinputdialog.h"
#include "movieelement.h"
#include "audioelement.h"
#include "lineelement.h"
#include "viewwidget.h"
#include "plugindialog.h"
#include "plugin.h"
#include "geometrydialog.h"
#include "configuration.h"
#include "shared.h"

namespace Ui
{
	class MainWindow;
}

class CFISLIDES_DLLSPEC MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QString commandLineHelp = QString(), QString openFile = QString(), bool disablePlugins = true, QWidget *parent = 0);
	~MainWindow();
	void setWindowModified(const bool);
	void setWindowTitle(const QString &);

	Slideshow *getSlideshow() const;
	void displaySlideshow();
	bool closeSlideshow();
	void displaySlide(Slide *slide);
	void updateSlide(const int index);
	void updateIcon(const int index);
	void updateSlideTree(const int index);
	void updatePropertiesEditor(const SlideshowElement *element);
	void clearPropertiesEditor();
	QGraphicsItem *sceneItemFromIndex(const int index) const;
	void addElement(SlideElement *element);
	void moveElement(const int before, const int after);

	static bool validateSlideName(const QString &name);
	static bool validateElementName(const QString &name);

public slots:
	void newSlideshow();
	void createEmptySlide();
	bool saveSlideshow();
	bool saveSlideshowAs();
	bool openSlideshow(const QString knowPath = QString());
	void updateCurrentSlideTree();
	void updateCurrentPropertiesEditor();
	void updateSelectionActions();
	void updateMediaPreview();
	void renameSlide();
	void reRenderSlide();
	void deleteSlide();
	void slideModified();
	void slideElementMoved();
	void launchViewer();
	void copySlide();
	void selectPrevSlide();
	void selectNextSlide();
	void about();
	void managePlugins();
	void print();
	void moveFinishTimerTimeout();
	void displayContextMenu(const QPoint &);
	void loadPlugins();
	void unloadPlugins();
	void aboutPlugins();
	void aboutQt();
	void displayRecentFiles();
	void openRecentFile(QAction *);
	void displayInsertElemMenu();
	void resizeSlideshow();

	void addImageElement();
	void addRectElement();
	void addEllipseElement();
	void addTextElement();
	void addMovieElement();
	void addAudioElement();
	void addLineElement();

	void currentSlideChanged(int currentRow);
	void slideItemChanged(QListWidgetItem *item);
	void elementItemChanged(QTreeWidgetItem *item, int column);
	void elementSelectionChanged();
	void deleteElements();
	void raiseElement();
	void lowerElement();
	void bringElementToFront();
	void bringElementToBack();
	void moveSlideLeft();
	void moveSlideRight();
	void duplicateElements();

signals:
	void insertElemMenu(QMenu *);

private:
	Ui::MainWindow *ui;

protected:
	Slideshow *slideshow;
	int newSlideshowCount;
	QTimer moveFinishTimer;
	void setInsertElemMenu(QMenu *menu);
	QList<QPluginLoader *> plugins;
	QString commandLineHelp;
	QActionGroup *currentSlideActions;
	QActionGroup *selectionActions;
	virtual void closeEvent(QCloseEvent *);
};

#endif // MAINWINDOW_H

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
#include <QShortcut>
#include <QPrinter>
#include <QPrintDialog>
#include <QTimer>
#include <QPluginLoader>
#include <QActionGroup>
#include <QMediaPlayer>
#include <QJsonObject>

#include "graphicsview.h"
#include "slideshow.h"
#include "slide.h"
#include "slideelement.h"
#include "imageelement.h"
#include "rectelement.h"
#include "ellipseelement.h"
#include "textelement.h"
#include "videoelement.h"
#include "audioelement.h"
#include "lineelement.h"
#include "viewwidget.h"
#include "plugindialog.h"
#include "plugin.h"
#include "geometrydialog.h"
#include "configuration.h"

namespace Ui
{
	class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	enum AlignDirection
	{
		ALIGN_LEFT,
		ALIGN_CENTER,
		ALIGN_RIGHT,
		ALIGN_TOP,
		ALIGN_MIDDLE,
		ALIGN_BOTTOM
	};

	explicit MainWindow(QString commandLineHelp = QString(), QString openFile = QString(), bool disablePlugins = true, QWidget *parent = 0);
	~MainWindow();

	Q_INVOKABLE void setWindowModified(const bool);
	Q_INVOKABLE void setWindowTitle(const QString &);
	Q_INVOKABLE Slideshow *getSlideshow() const;
	Q_INVOKABLE bool closeSlideshow();
	Q_INVOKABLE void displaySlide(Slide *slide);
	Q_INVOKABLE void updateSlide(const int index);
	Q_INVOKABLE void updateIcon(const int index);
	Q_INVOKABLE void updateSlideTree(const int index);
	Q_INVOKABLE QGraphicsItem *sceneItemFromIndex(const int index) const;
	Q_INVOKABLE void insertElement(SlideElement *element);
	Q_INVOKABLE void alignElementsTo(const AlignDirection direction);

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
	void launchViewerFromCurrentSlide();
	void launchViewerFromStart();
	void copySlide();
	void selectPrevSlide();
	void selectNextSlide();
	void about();
	void managePlugins();
	void print();
	void moveFinishTimerTimeout();
	void loadPlugins();
	void unloadPlugins();
	void aboutPlugins();
	void aboutQt();
	void displayRecentFiles();
	void openRecentFile(QAction *);
	void displayInsertElemMenu();
	void resizeSlideshow();
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
	void playPreview();
	void pausePreview();
	void setPreviewVolume(int);
	void setPreviewPosition(int);
	void previewSeekableChanged(bool);
	void previewDurationChanged(qint64);
	void previewPositionChanged(qint64);
	void previewStateChanged(QMediaPlayer::State);
	void alignElementsToCenter();
	void alignElementsToMiddle();

private:
	void updatePropertiesEditor(const SlideshowElement *element);
	void moveElement(const int before, const int after);
	bool validateSlideName(const QString &name);
	bool validateElementName(const QString &name);
	void registerElementType(const SlideElementType &type);
	void unregisterElementType(const SlideElementType &type);
	QMenu *createSlideContextMenu();
	QString msToString(const int ms) const;
	void launchViewer(const int from);
	void appendToRecentFiles(const QString openedFile);

	Ui::MainWindow *ui;
	Slideshow *slideshow;
	int newSlideshowCount;
	QTimer moveFinishTimer;
	QList<QAction *> insertActions;
	QList<QPluginLoader *> plugins;
	QString commandLineHelp;
	QActionGroup *currentSlideActions;
	QActionGroup *selectionActions;
	SlideElementTypeList pluginTypes;
	QMediaPlayer *previewPlayer;

private slots:
	void displayViewContextMenu(const QPoint &);
	void displaySlideTreeContextMenu(const QPoint &);
	void displaySlideListContextMenu(const QPoint &pos);
	void insertElementFromAction();

protected:
	virtual void closeEvent(QCloseEvent *);
};

#endif // MAINWINDOW_H

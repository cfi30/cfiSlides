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
#include <QTimer>
#include <QElapsedTimer>
#include <QMediaPlayer>

#include "slideelementtype.h"

class QGraphicsItem;
class QTreeWidgetItem;
class QListWidgetItem;
class QPluginLoader;
class QActionGroup;

namespace Ui
{
	class MainWindow;
}

class Slideshow;
class SlideshowElement;
class Slide;
class SlideElement;

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
	Q_INVOKABLE void renderSlide(const int index);
	Q_INVOKABLE void updateSlideIcon(const int index);
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
	void refreshSlide();
	void deleteSlide();
	void slideModified();
	void slideElementMoved();
	void launchViewerFromCurrentSlide();
	void launchViewerFromStart();
	void duplicateSlide();
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
	void populateRecentFilesMenu();
	void openRecentFile(QAction *);
	void populateInsertMenu();
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
	void cutElements();
	void copyElements();
	void pasteElements();

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
	void clearClipboard();

	Ui::MainWindow *ui;
	Slideshow *slideshow;
	int newSlideshowCount;
	QTimer moveFinishTimer;
	QList<QAction *> insertActions;
	QList<QPluginLoader *> plugins;
	QString commandLineHelp;
	QActionGroup *slideActions;
	QActionGroup *selectionActions;
	SlideElementTypeList pluginTypes;
	QMediaPlayer *previewPlayer;
	QElapsedTimer viewerTimer;
	QList<SlideElement *> clipboard;

private slots:
	void displayViewContextMenu(const QPoint &);
	void displaySlideTreeContextMenu(const QPoint &);
	void displaySlideListContextMenu(const QPoint &pos);
	void insertElementFromAction();
	void viewerClosed();

protected:
	virtual void closeEvent(QCloseEvent *);
};

#endif // MAINWINDOW_H

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

#ifndef VIEWWIDGET_H
#define VIEWWIDGET_H

#include <QWidget>
#include <QDesktopWidget>
#include <QGraphicsView>
#include <QProgressDialog>
#include <QInputDialog>
#include <QShortcut>

#include "slideshow.h"
#include "shared.h"

namespace Ui
{
	class ViewWidget;
}

class ViewWidget : public QWidget
{
	Q_OBJECT
	
public:
	explicit ViewWidget(QWidget *parent = 0);
	~ViewWidget();
	void setSlideshow(Slideshow *slideshow, const int startIndex = 0);
	
signals:
	void closed(int);

public slots:
	void prev();
	void next();
	void first();
	void last();
	void play();
	void pause();
	void togglePlayPause();
	void stop();
	void toggleMute();
	void destroy();
	void restart();
	void goTo();

private:
	Ui::ViewWidget *ui;

protected:
	bool paused;
	Slideshow *slideshow;
	void closeEvent(QCloseEvent *);
};

#endif // VIEWWIDGET_H

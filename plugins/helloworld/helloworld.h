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

#ifndef HELLOWORLD_H
#define HELLOWORLD_H

#include <QMessageBox>
#include <QMenuBar>
#include <QMenu>
#include <QToolBar>
#include <QApplication>
#include <QDesktopServices>
#include <QUrl>

#include "plugin.h"
#include "helloelement.h"

class HelloWorld : public Plugin
{
	Q_OBJECT
	Q_INTERFACES(Plugin)

public:
	HelloWorld();
	virtual ~HelloWorld();
	virtual QString name() const;
	virtual QString version() const;
	virtual QString author() const;
	virtual QString about() const;
	virtual void initialize();
	virtual SlideElementTypeList getElementTypes() const;

public slots:
	void doSomething();
	void doNothing();

protected:
	QMenu *helloMenu;
	QAction *helloAction;
	QAction *hiAction;
	int hiActionState;
};

#endif // HELLOWORLD_H

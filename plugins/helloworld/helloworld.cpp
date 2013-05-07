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

#include "helloworld.h"

HelloWorld::HelloWorld()
{
	helloMenu = new QMenu("Hello World");
	helloAction = helloMenu->addAction(QIcon::fromTheme("call-start"), "Do something !", this, SLOT(doSomething()));
	helloAction->setCheckable(true);
	hiAction = helloMenu->addAction(QIcon::fromTheme("call-stop"), "This one won't do a thing.", this, SLOT(doNothing()));

	elementAction = new QAction(QIcon::fromTheme("applications-internet"), "Hello World", this);
	connect(elementAction, SIGNAL(triggered()), this, SLOT(addHelloElement()));

	hiActionState = false;
}

void HelloWorld::initialize()
{
	qRegisterMetaType<HelloElement>("HelloElement");

	window->menuBar()->insertMenu(window->findChild<QMenu *>("menuConfiguration")->menuAction(), helloMenu);
	window->findChild<QToolBar *>("toolBar")->addAction(helloAction);
	connect(window, SIGNAL(insertElemMenu(QMenu *)), this, SLOT(insertElemMenu(QMenu *)));
}

HelloWorld::~HelloWorld()
{
	QMetaType::unregisterType("HelloElement");

	delete helloMenu;
	delete elementAction;
}

QString HelloWorld::name() const
{
	return "Hello World";
}

QString HelloWorld::version() const
{
	return "1.0";
}

QString HelloWorld::author() const
{
	return "<a href=\"http://mysite.com\">Christian Fillion</a>";
}

QString HelloWorld::about() const
{
	return "Demo plugin for cfiSlides.";
}

void HelloWorld::doSomething()
{
	if(helloAction->isChecked())
		QMessageBox::information(window, "Hello World", "A wild message just popped up.");
	else
		QMessageBox::critical(window, "Hello World", "Error #229: wrong polarity of neutron flow.");
}

void HelloWorld::doNothing()
{
	if(!helloAction->isChecked())
		return;

	switch(++hiActionState)
	{
		case 1:
			QMessageBox::critical(window, "Error", "There is nothing to see here.");
			break;
		case 2:
			QMessageBox::critical(window, "Fatal Error", "Really, there is absolutely nothing here.");
			break;
		case 3:
			QMessageBox::critical(window, "FUCKING FATAL ERROR", "Why don't you believe me and quit already? Go buy a coffee instead. Or a pizza.");
			hiAction->setText("This one REALLY won't do a thing.");
			break;
		case 4:
			hiAction->setText("This one REALLY will do a thing.");
			break;
		case 5:
			QMessageBox::about(window, "...", "No comment.");
			hiAction->setText("Click here");
			break;
		case 6:
			QMessageBox::information(window, "I_LOVE_CANDIES!!1", "You clicked on the menu 6 times. Make it 12 will you?", QMessageBox::No);
			break;
		case 7:
		case 8:
			break;
		case 9:
			hiAction->setText("3...");
			break;
		case 10:
			hiAction->setText("2...");
			break;
		case 11:
			hiAction->setText("One last...");
			break;
		case 12:
			QMessageBox::question(window, "Never gonna say goodbye.", "Lets sing a song together.", QMessageBox::Abort);
			QDesktopServices::openUrl(QUrl::fromPercentEncoding("http://youtu.be/oHg5SJYRHA0"));
			hiAction->setText("");
			break;
		default:
			QDesktopServices::openUrl(QUrl::fromPercentEncoding("http://youtu.be/yzC4hFK5P3g"));
			helloAction->setChecked(false);
			hiAction->setDisabled(true);
			break;
	}
}

void HelloWorld::insertElemMenu(QMenu *menu)
{
	menu->addAction(elementAction);
}

void HelloWorld::addHelloElement()
{
	HelloElement *element = new HelloElement;
	element->setValue("name", "HelloWorld %1");

	window->addElement(element);
}

Q_EXPORT_PLUGIN2(hello_world, HelloWorld)
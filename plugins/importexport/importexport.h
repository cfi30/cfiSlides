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

#ifndef IMPORTEXPORT_H
#define IMPORTEXPORT_H

#include <QObject>
#include "plugin.h"

class ImportExport : public Plugin
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "ImportExport" FILE "importexport.json")
	Q_INTERFACES(Plugin)

public:
	ImportExport();
	virtual ~ImportExport();
	virtual void initialize();

public slots:
	void launchImport();
	void launchExport();

protected:
	QAction *importAction;
	QAction *exportAction;
};

#endif // IMPORTEXPORT_H

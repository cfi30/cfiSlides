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

#ifndef TEXTEDITFACTORY_H
#define TEXTEDITFACTORY_H

#include <QtStringPropertyManager>

#include "textedit.h"
#include "shared.h"

class CFISLIDES_DLLSPEC TextEditFactory : public QtAbstractEditorFactory<QtStringPropertyManager>
{
	Q_OBJECT
public:
	TextEditFactory(QObject *parent = 0);
	virtual ~TextEditFactory();
	
protected:
	virtual void connectPropertyManager(QtStringPropertyManager *manager);
	virtual QWidget *createEditor(QtStringPropertyManager *manager, QtProperty *property, QWidget *parent);
	virtual void disconnectPropertyManager(QtStringPropertyManager *manager);

private slots:
	void slotPropertyChanged(QtProperty *property, const QString &value);
	void slotSetValue(const QString &value);
	void slotEditorDestroyed(QObject *object);

private:
	QMap<QtProperty *, QList<TextEdit *> > theCreatedEditors;
	QMap<TextEdit *, QtProperty *> theEditorToProperty;
	
};

#endif // TEXTEDITFACTORY_H

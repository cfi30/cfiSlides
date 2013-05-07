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

#ifndef BASEELEMENT_H
#define BASEELEMENT_H

#include <QObject>
#include <QList>
#include <QtProperty>
#include <QtTreePropertyBrowser>
#include <QtStringPropertyManager>
#include <QtLineEditFactory>
#include <QGraphicsScene>

#include "shared.h"

class CFISLIDES_DLLSPEC BaseElement : public QObject
{
	Q_OBJECT
public:
	explicit BaseElement(QObject *parent = 0);
	virtual void bindProperties(QtTreePropertyBrowser *browser) const;
	QVariant getValue(const QString &name, QVariant defaultValue = QVariant()) const;
	void setValue(const QString &name, QVariant value);
	QMap<QString, QVariant> getProperties() const;
	void setProperties(QMap<QString, QVariant>);
	virtual void render(QGraphicsScene *scene, const bool interactive) = 0;
	
signals:
	void modified();

protected slots:
	void valueChanged(QtProperty *, QVariant);
	void stringValueChanged(QtProperty *, QString);
	void intValueChanged(QtProperty *, int);
	void colorValueChanged(QtProperty *, QColor);
	void sizeValueChanged(QtProperty *, QSize);
	void pointValueChanged(QtProperty *, QPoint);
	void fontValueChanged(QtProperty *, QFont);
	void boolValueChanged(QtProperty *, bool);

private:
	QMap<QString, QVariant> properties;
};
#endif // BASEELEMENT_H

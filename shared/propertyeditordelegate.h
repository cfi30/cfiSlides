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

#ifndef PROPERTYDELEGATE_H
#define PROPERTYDELEGATE_H

#include <QItemDelegate>
#include <QPainter>
#include <QApplication>
#include <QFocusEvent>

#include "property.h"
#include "shared.h"

class CFISLIDES_DLLSPEC PropertyEditorDelegate : public QItemDelegate
{
	Q_OBJECT

public:
	explicit PropertyEditorDelegate(QObject *parent) : QItemDelegate(parent) {}
	void registerProperty(Property *);
	void deleteProperties();
	Property *propertyAt(const int index) const;
	int size() const;
	virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &index) const;
	virtual void setEditorData(QWidget *, const QModelIndex &) const {}
	virtual void setModelData(QWidget *, QAbstractItemModel *, const QModelIndex &) const;
	virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
	virtual void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	virtual bool eventFilter(QObject *object, QEvent *event);

signals:
	void itemChanged(int) const;

private slots:
	void propertyModified(QString, QVariant);

protected:
	PropertyList properties;
	QMap<QString, Property *> nameToProp;
};

#endif // PROPERTYDELEGATE_H

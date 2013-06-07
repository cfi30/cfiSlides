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

#ifndef PROPERTYEDITOR_H
#define PROPERTYEDITOR_H

#include <QTreeWidget>
#include <QLayout>
#include <QHeaderView>

#include "property.h"
#include "propertyeditordelegate.h"
#include "shared.h"

class CFISLIDES_DLLSPEC PropertyEditorView : public QTreeWidget
{
public:
	explicit PropertyEditorView(QWidget *parent = 0) : QTreeWidget(parent) {}

protected:
	virtual void drawRow(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	virtual void keyPressEvent(QKeyEvent *event);
	virtual void mousePressEvent(QMouseEvent *event);
};

class CFISLIDES_DLLSPEC PropertyEditor : public QWidget
{
	Q_OBJECT

public:
	explicit PropertyEditor(QWidget *parent = 0);
	~PropertyEditor();
	void addTopLevelProperty(Property *prop);
	void addTopLevelProperties(PropertyList props);
	void clear();

private slots:
	void itemChanged(int);

private:
	void updateItem(QTreeWidgetItem *, const Property *);
	QTreeWidgetItem *addProperty(Property *, const bool isRoot);
	PropertyEditorView *treeWidget;
	PropertyEditorDelegate *delegate;
	QMap<int, QTreeWidgetItem *> indexToItem;
};

#endif // PROPERTYEDITOR_H

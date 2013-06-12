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

#include "propertyeditor.h"

PropertyEditor::PropertyEditor(QWidget *parent) : QWidget(parent)
{
	delegate = new PropertyEditorDelegate(this);
	connect(delegate, &PropertyEditorDelegate::itemChanged, this, &PropertyEditor::itemChanged);

	treeWidget = new PropertyEditorView(this);
	treeWidget->setColumnCount(2);
	treeWidget->setHeaderLabels(QStringList() << tr("Paramètre") << tr("Valeur"));
	treeWidget->setAlternatingRowColors(true);
	treeWidget->setRootIsDecorated(false);
	treeWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	treeWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	treeWidget->setItemDelegate(delegate);

	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->setMargin(0);
	layout->addWidget(treeWidget);
}

PropertyEditor::~PropertyEditor()
{
	clear();
}

void PropertyEditor::addTopLevelProperty(Property *prop)
{
	addProperty(prop, true);
}

void PropertyEditor::addTopLevelProperties(PropertyList props)
{
	foreach(Property *prop, props)
		addTopLevelProperty(prop);
}

QTreeWidgetItem *PropertyEditor::addProperty(Property *prop, const bool isRoot)
{
	const int index = delegate->size();

	QTreeWidgetItem *item = new QTreeWidgetItem;
	item->setFlags(item->flags() | Qt::ItemIsEditable);
	item->setText(0, prop->getLabel());
	item->setToolTip(0, prop->getToolTip());
	item->setData(1, Qt::UserRole, index);

	updateItem(item, prop);

	delegate->registerProperty(prop);
	indexToItem[index] = item;

	foreach(Property *subprop, prop->getProperties())
	{
		QTreeWidgetItem *child = addProperty(subprop, false);
		item->addChild(child);
		treeWidget->setItemExpanded(child, true);
		child->setFirstColumnSpanned(!subprop->getManager());
	}

	if(isRoot)
	{
		treeWidget->addTopLevelItem(item);
		treeWidget->setItemExpanded(item, true);
		item->setFirstColumnSpanned(!prop->getManager());
	}

	return item;
}

void PropertyEditor::itemChanged(int index)
{
	updateItem(indexToItem[index], delegate->propertyAt(index));
}

void PropertyEditor::clear()
{
	treeWidget->clear();
	this->delegate->deleteProperties();
	indexToItem.clear();
}

void PropertyEditor::updateItem(QTreeWidgetItem *item, const Property *prop)
{
	PropertyManager *manager = prop->getManager();

	QFont font;
	font.setBold(!manager);
	item->setFont(0, font);

	if(!manager) return;

	QString name = prop->getName();
	QVariant value = prop->getValue();
	QString stringValue = manager->valueToString(name, value);
	item->setText(1, stringValue);
	item->setToolTip(1, stringValue);
	item->setIcon(1, manager->valueToIcon(name, value));
}

void PropertyEditorView::drawRow(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QStyleOptionViewItem opt = option;

	PropertyEditorDelegate *delegate = qobject_cast<PropertyEditorDelegate *>(itemDelegate());
	Property *prop = delegate->propertyAt(this->itemFromIndex(index)->data(1, Qt::UserRole).toInt());
	PropertyManager *manager = prop->getManager();
	if(manager && !manager->isValid(prop->getName(), prop->getValue()))
	{
		QColor color = QColor(Qt::red);
		color.setAlpha(142);

		painter->fillRect(option.rect, color);
		opt.palette.setColor(QPalette::AlternateBase, color.lighter(112));
	}

	QTreeWidget::drawRow(painter, opt, index);

	const QColor borderColor = static_cast<QRgb>(QApplication::style()->styleHint(QStyle::SH_Table_GridLineColor, &option));

	painter->save();
	painter->setPen(QPen(borderColor));
	painter->drawLine(opt.rect.x(), opt.rect.bottom(), opt.rect.right(), opt.rect.bottom());
	painter->restore();
}

void PropertyEditorView::keyPressEvent(QKeyEvent *event)
{
	switch(event->key())
	{
		case Qt::Key_Return:
		case Qt::Key_Enter:
		case Qt::Key_Space:
			QTreeWidgetItem *item = currentItem();
			if(item && !item->isFirstColumnSpanned() && state() != QAbstractItemView::EditingState)
			{
				editItem(item, 1);
				setCurrentItem(item, 1);
				return;
			}
			break;
	}

	QTreeWidget::keyPressEvent(event);
}

void PropertyEditorView::mousePressEvent(QMouseEvent *event)
{
	QTreeView::mousePressEvent(event);
	QTreeWidgetItem *item = itemAt(event->pos());

	if(item && !item->isFirstColumnSpanned() && header()->logicalIndexAt(event->pos().x()) == 1 && state() != QAbstractItemView::EditingState)
	{
		editItem(item, 1);
		setCurrentItem(item, 1);
	}
}

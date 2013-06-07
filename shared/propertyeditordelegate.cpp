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

#include "propertyeditordelegate.h"

void PropertyEditorDelegate::registerProperty(Property *prop)
{
	properties << prop;

	PropertyManager *manager = prop->getManager();
	if(manager)
	{
		connect(manager, SIGNAL(modified(QString, QVariant)), this, SLOT(propertyModified(QString, QVariant)));
		nameToProp[prop->getName()] = prop;
	}
}

void PropertyEditorDelegate::deleteProperties()
{
	while(!properties.isEmpty())
		delete properties.takeFirst();
}

int PropertyEditorDelegate::size() const
{
	return properties.size();
}

Property *PropertyEditorDelegate::propertyAt(const int index) const
{
	return properties[index];
}

QWidget *PropertyEditorDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &index) const
{
	if(index.column() != 1)
		return 0;

	Property *prop = properties[index.data(Qt::UserRole).toInt()];
	PropertyManager *manager = prop->getManager();
	if(!manager) return 0;

	QWidget *editor = manager->createEditor(prop->getName(), prop->getValue(), parent);
	if(editor)
	{
		editor->setAutoFillBackground(true);
		editor->installEventFilter(const_cast<PropertyEditorDelegate *>(this));
	}
	return editor;
}

void PropertyEditorDelegate::setModelData(QWidget *, QAbstractItemModel *, const QModelIndex &index) const
{
	emit itemChanged(index.data(Qt::UserRole).toInt());
}

void PropertyEditorDelegate::propertyModified(QString propName, QVariant newValue)
{
	nameToProp[propName]->setValue(newValue);
}

void PropertyEditorDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QStyleOptionViewItem opt = option;
	opt.state &= ~QStyle::State_HasFocus;
	opt.palette.setCurrentColorGroup(QPalette::Active);

	QItemDelegate::paint(painter, opt, index);

	const QColor bcolor = static_cast<QRgb>(QApplication::style()->styleHint(QStyle::SH_Table_GridLineColor, &opt));
	painter->save();
	painter->setPen(QPen(bcolor));

	const int right = (option.direction == Qt::LeftToRight) ? option.rect.right() : option.rect.left();
	painter->drawLine(right, option.rect.y(), right, option.rect.bottom());
	painter->restore();
}

QSize PropertyEditorDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	return QItemDelegate::sizeHint(option, index) + QSize(3, 4);
}

void PropertyEditorDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &) const
{
	editor->setGeometry(option.rect.adjusted(0, 0, 0, -1));
}

bool PropertyEditorDelegate::eventFilter(QObject *object, QEvent *event)
{
	if(event->type() == QEvent::FocusOut)
	{
		QFocusEvent *fe = static_cast<QFocusEvent *>(event);
		if(fe->reason() == Qt::ActiveWindowFocusReason)
			return false;
	}

	return QItemDelegate::eventFilter(object, event);
}

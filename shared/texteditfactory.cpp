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

#include "texteditfactory.h"

TextEditFactory::TextEditFactory(QObject *parent) : QtAbstractEditorFactory<QtStringPropertyManager>(parent)
{
}

TextEditFactory::~TextEditFactory()
{
	QList<TextEdit *> editors = theEditorToProperty.keys();
	QListIterator<TextEdit *> it(editors);
	while (it.hasNext())
		delete it.next();
}

void TextEditFactory::connectPropertyManager(QtStringPropertyManager *manager)
{
	connect(manager, SIGNAL(valueChanged(QtProperty *, const QString &)),
			this, SLOT(slotPropertyChanged(QtProperty *, const QString &)));
}

QWidget *TextEditFactory::createEditor(QtStringPropertyManager *manager, QtProperty *property, QWidget *parent)
{
	TextEdit *editor = new TextEdit(parent);
	editor->setText(manager->value(property));
	theCreatedEditors[property].append(editor);
	theEditorToProperty[editor] = property;

	connect(editor, SIGNAL(textChanged(const QString &)),
			this, SLOT(slotSetValue(const QString &)));
	connect(editor, SIGNAL(destroyed(QObject *)),
			this, SLOT(slotEditorDestroyed(QObject *)));
	return editor;
}

void TextEditFactory::disconnectPropertyManager(QtStringPropertyManager *manager)
{
	disconnect(manager, SIGNAL(valueChanged(QtProperty *, const QString &)),
			   this, SLOT(slotPropertyChanged(QtProperty *, const QString &)));
	disconnect(manager, SIGNAL(filterChanged(QtProperty *, const QString &)),
			   this, SLOT(slotFilterChanged(QtProperty *, const QString &)));
}

void TextEditFactory::slotPropertyChanged(QtProperty *property,
										  const QString &value)
{
	if (!theCreatedEditors.contains(property))
		return;

	QList<TextEdit *> editors = theCreatedEditors[property];
	QListIterator<TextEdit *> itEditor(editors);
	while (itEditor.hasNext())
		itEditor.next()->setText(value);
}

void TextEditFactory::slotSetValue(const QString &value)
{
	QObject *object = sender();
	QMap<TextEdit *, QtProperty *>::ConstIterator itEditor =
			theEditorToProperty.constBegin();
	while (itEditor != theEditorToProperty.constEnd()) {
		if (itEditor.key() == object) {
			QtProperty *property = itEditor.value();
			QtStringPropertyManager *manager = propertyManager(property);
			if (!manager)
				return;
			manager->setValue(property, value);
			return;
		}
		itEditor++;
	}
}

void TextEditFactory::slotEditorDestroyed(QObject *object)
{
	QMap<TextEdit *, QtProperty *>::ConstIterator itEditor =
			theEditorToProperty.constBegin();
	while (itEditor != theEditorToProperty.constEnd()) {
		if (itEditor.key() == object) {
			TextEdit *editor = itEditor.key();
			QtProperty *property = itEditor.value();
			theEditorToProperty.remove(editor);
			theCreatedEditors[property].removeAll(editor);
			if (theCreatedEditors[property].isEmpty())
				theCreatedEditors.remove(property);
			return;
		}
		itEditor++;
	}
}

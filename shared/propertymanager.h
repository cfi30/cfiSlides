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

#ifndef PROPERTYMANAGER_H
#define PROPERTYMANAGER_H

#include "limits.h"

#include <QVariant>
#include <QIcon>
#include <QLineEdit>
#include <QRegExpValidator>
#include <QLayout>
#include <QLabel>
#include <QToolButton>
#include <QColorDialog>
#include <QFileDialog>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QPainter>
#include <QFontDialog>

#include "textinputdialog.h"
#include "shared.h"

class CFISLIDES_DLLSPEC PropertyManager : public QObject
{
	Q_OBJECT

public:
	PropertyManager() : QObject() {}
	virtual ~PropertyManager() {}
	virtual QString valueToString(const QString &propName, const QVariant &value) const;
	virtual QIcon valueToIcon(const QString &propName, const QVariant &value) const;
	virtual QWidget *createEditor(const QString &propName, const QVariant &value, QWidget *parent);
	virtual bool isValid(const QString &propName, const QVariant &value) const;
	void setRequired(const QString &propName, const bool required);
	void setReadOnly(const QString &propName, const bool readOnly);

signals:
	void modified(const QString &, const QVariant &);

protected:
	QString currentProperty;
	QMap<QString, bool> isRequired;
	QMap<QString, bool> isReadOnly;
};

class CFISLIDES_DLLSPEC StringPropertyManager : public PropertyManager
{
	Q_OBJECT

public:
	virtual QString valueToString(const QString &propName, const QVariant &value) const;
	virtual QWidget *createEditor(const QString &, const QVariant &, QWidget *);
	virtual bool isValid(const QString &propName, const QVariant &value) const;
	void setRegExp(const QString &propName, const QRegExp &regex);
	void setPlaceholder(const QString &propName, const QString &placeholder);

protected:
	QMap<QString, QRegExp> regexes;
	QMap<QString, QString> placeholders;

protected:
	QLineEdit *editor;

protected slots:
	void textChanged(QString);
};

class CFISLIDES_DLLSPEC TextPropertyManager : public StringPropertyManager
{
	Q_OBJECT

public:
	virtual QString valueToString(const QString &, const QVariant &) const;
	virtual QWidget *createEditor(const QString &, const QVariant &, QWidget *);

protected slots:
	void buttonClicked();
};

class CFISLIDES_DLLSPEC ColorPropertyManager : public PropertyManager
{
	Q_OBJECT

public:
	virtual QString valueToString(const QString &, const QVariant &) const;
	virtual QIcon valueToIcon(const QString &, const QVariant &) const;
	virtual QWidget *createEditor(const QString &, const QVariant &, QWidget *);

protected:
	QString colorToString(const QColor &color) const;
	QPixmap colorToPixmap(const QColor &color) const;

private slots:
	void buttonClicked();

private:
	QColor currentColor;
	QLabel *colorPreview;
	QLabel *colorLabel;
};

class CFISLIDES_DLLSPEC FilePropertyManager : public PropertyManager
{
	Q_OBJECT

public:
	virtual QString valueToString(const QString &, const QVariant &) const;
	virtual QWidget *createEditor(const QString &, const QVariant &, QWidget *);
	virtual bool isValid(const QString &propName, const QVariant &value) const;
	void setFilter(const QString &propName, const QString &filter);

protected:
	QMap<QString, QString> filters;

protected slots:
	void filePathChanged(QString);

private:
	QLineEdit *fileEdit;

private slots:
	void buttonClicked();
};

class CFISLIDES_DLLSPEC EnumPropertyManager : public PropertyManager
{
	Q_OBJECT

public:
	virtual QString valueToString(const QString &, const QVariant &) const;
	virtual QWidget *createEditor(const QString &, const QVariant &, QWidget *);
	void setEnumNames(const QString &propName, const QStringList &names);

protected:
	QMap<QString, QStringList> enumNames;

protected slots:
	void currentValueChanged(int);
};

class CFISLIDES_DLLSPEC BoolPropertyManager : public PropertyManager
{
	Q_OBJECT

public:
	virtual QString valueToString(const QString &, const QVariant &) const;
	virtual QWidget *createEditor(const QString &, const QVariant &, QWidget *);

private:
	QCheckBox *button;

protected slots:
	void buttonToogled(bool);
};

class CFISLIDES_DLLSPEC SizePropertyManager : public PropertyManager
{
	Q_OBJECT

public:
	virtual QString valueToString(const QString &, const QVariant &) const;
	virtual QWidget *createEditor(const QString &, const QVariant &, QWidget *);
	void setMinimum(const QString &propName, const QSize &min);
	void setMaximum(const QString &propName, const QSize &max);

protected:
	QMap<QString, QSize> minimums;
	QMap<QString, QSize> maximums;

private:
	QSpinBox *width;
	int widthCache;
	QSpinBox *height;
	int heightCache;
	QToolButton *lock;

private slots:
	void widthChanged(int);
	void heightChanged(int);
	void valuesChanged();
};

class CFISLIDES_DLLSPEC PointPropertyManager : public PropertyManager
{
	Q_OBJECT

public:
	virtual QString valueToString(const QString &, const QVariant &) const;
	virtual QWidget *createEditor(const QString &, const QVariant &, QWidget *);
	void setMinimum(const QString &propName, const QPoint &min);
	void setMaximum(const QString &propName, const QPoint &max);

protected:
	QMap<QString, QPoint> minimums;
	QMap<QString, QPoint> maximums;

private:
	QSpinBox *x;
	QSpinBox *y;

private slots:
	void valuesChanged();
};

class CFISLIDES_DLLSPEC IntPropertyManager : public PropertyManager
{
	Q_OBJECT

public:
	virtual QString valueToString(const QString &, const QVariant &) const;
	virtual QWidget *createEditor(const QString &, const QVariant &, QWidget *);
	void setMinimum(const QString &propName, const int min);
	void setMaximum(const QString &propName, const int max);
	void setPrefix(const QString &propName, const QString &prefix);
	void setSuffix(const QString &propName, const QString &suffix);

protected:
	QMap<QString, int> minimums;
	QMap<QString, int> maximums;
	QMap<QString, QString> prefixes;
	QMap<QString, QString> suffixes;
	QMap<QString, int> editors;

protected slots:
	void valueChanged(int);
};

class CFISLIDES_DLLSPEC IntSliderPropertyManager : public IntPropertyManager
{
	Q_OBJECT

public:
	virtual QWidget *createEditor(const QString &, const QVariant &, QWidget *);
};

class CFISLIDES_DLLSPEC FontPropertyManager : public PropertyManager
{
	Q_OBJECT

public:
	virtual QString valueToString(const QString &, const QVariant &) const;
	virtual QIcon valueToIcon(const QString &, const QVariant &) const;
	virtual QWidget *createEditor(const QString &, const QVariant &, QWidget *);

protected:
	QString fontToString(const QFont &font) const;
	QPixmap fontToPixmap(const QFont &font) const;

private slots:
	void buttonClicked();

private:
	QFont currentFont;
	QLabel *fontPreview;
	QLabel *fontLabel;
};

#endif // PROPERTYMANAGER_H

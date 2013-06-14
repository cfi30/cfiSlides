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

#include "propertymanager.h"

QString PropertyManager::valueToString(const QString &, const QVariant &value) const
{
	return value.toString();
}

QIcon PropertyManager::valueToIcon(const QString &, const QVariant &) const
{
	return QIcon();
}

QWidget *PropertyManager::createEditor(const QString &, const QVariant &, QWidget *)
{
	return 0;
}

void PropertyManager::setRequired(const QString &propName, const bool required)
{
	isRequired[propName] = required;
}

void PropertyManager::setReadOnly(const QString &propName, const bool readOnly)
{
	isReadOnly[propName] = readOnly;
}

bool PropertyManager::isValid(const QString &propName, const QVariant &value) const
{
	return isRequired[propName] ? value.isValid() : true;
}

QString StringPropertyManager::valueToString(const QString &propName, const QVariant &value) const
{
	const QString svalue = value.toString();
	return svalue.isEmpty() ? placeholders[propName] : svalue;
}

QWidget *StringPropertyManager::createEditor(const QString &propName, const QVariant &value, QWidget *parent)
{
	if(isReadOnly[propName]) return 0;
	currentProperty = propName;

	editor = new QLineEdit(value.toString(), parent);
	if(regexes.contains(propName))
		editor->setValidator(new QRegExpValidator(regexes[propName], editor));

	connect(editor, &QLineEdit::textChanged, this, &StringPropertyManager::textChanged);

	return editor;
}

bool StringPropertyManager::isValid(const QString &propName, const QVariant &value) const
{
	return isRequired[propName] ? !value.toString().isEmpty() : true;
}

void StringPropertyManager::setRegExp(const QString &propName, const QRegExp &regex)
{
	regexes[propName] = regex;
}

void StringPropertyManager::setPlaceholder(const QString &propName, const QString &placeholder)
{
	placeholders[propName] = placeholder;
}

void StringPropertyManager::textChanged(QString newValue)
{
	if(editor->hasAcceptableInput())
		emit modified(currentProperty, newValue);
}

QString TextPropertyManager::valueToString(const QString &, const QVariant &value) const
{
	return value.toString().replace("\n", " ");
}

QWidget *TextPropertyManager::createEditor(const QString &propName, const QVariant &value, QWidget *parent)
{
	if(isReadOnly[propName]) return 0;
	currentProperty = propName;

	QWidget *widget = new QWidget(parent);
	QHBoxLayout *layout = new QHBoxLayout(widget);
	layout->setMargin(0);

	editor = new QLineEdit(value.toString(), widget);
	layout->addWidget(editor);

	QToolButton *button = new QToolButton(widget);
	button->setText("...");
	button->setFixedWidth(20);
	layout->addWidget(button);

	connect(editor, &QLineEdit::textChanged, this, &TextPropertyManager::textChanged);
	connect(button, &QToolButton::clicked, this, &TextPropertyManager::buttonClicked);

	widget->setFocusProxy(editor);
	return widget;
}

void TextPropertyManager::buttonClicked()
{
	TextInputDialog *dialog = new TextInputDialog(editor);
	dialog->setText(editor->text());
	if(dialog->exec() == QDialog::Rejected)
		return;

	editor->setText(dialog->text());
}

QString ColorPropertyManager::valueToString(const QString &, const QVariant &value) const
{
	return colorToString(value.value<QColor>());
}

QIcon ColorPropertyManager::valueToIcon(const QString &, const QVariant &value) const
{
	return QIcon(colorToPixmap(value.value<QColor>()));
}

QWidget *ColorPropertyManager::createEditor(const QString &propName, const QVariant &value, QWidget *parent)
{
	if(isReadOnly[propName]) return 0;

	currentColor = value.value<QColor>();
	currentProperty = propName;

	QWidget *widget = new QWidget(parent);
	QHBoxLayout *layout = new QHBoxLayout(widget);
	layout->setMargin(0);

	colorPreview = new QLabel(widget);
	colorPreview->setPixmap(colorToPixmap(currentColor));
	layout->addWidget(colorPreview);

	colorLabel = new QLabel(colorToString(currentColor), widget);
	layout->addWidget(colorLabel);

	QToolButton *button = new QToolButton(widget);
	button->setText("...");
	button->setFixedWidth(20);
	layout->addWidget(button);

	connect(button, &QToolButton::clicked, this, &ColorPropertyManager::buttonClicked);

	widget->setFocusProxy(button);
	return widget;
}

void ColorPropertyManager::buttonClicked()
{
	QColorDialog dialog;
	dialog.setOptions(QColorDialog::ShowAlphaChannel);
	dialog.setCurrentColor(currentColor);
	if(dialog.exec() == QDialog::Rejected)
		return;

	currentColor = dialog.currentColor();
	colorPreview->setPixmap(colorToPixmap(currentColor));
	colorLabel->setText(colorToString(currentColor));

	emit modified(currentProperty, currentColor);
}

QString ColorPropertyManager::colorToString(const QColor &color) const
{
	return QString("[%1, %2, %3] (%4)")
		.arg(color.red())
		.arg(color.green())
		.arg(color.blue())
		.arg(color.alpha());
}

QPixmap ColorPropertyManager::colorToPixmap(const QColor &color) const
{
	QPixmap pixmap(16, 16);
	pixmap.fill(color);
	return pixmap;
}

QString FilePropertyManager::valueToString(const QString &, const QVariant &value) const
{
	return value.toString().isEmpty() ? tr("Aucun fichier") : QFileInfo(value.toString()).fileName();
}

QWidget *FilePropertyManager::createEditor(const QString &propName, const QVariant &value, QWidget *parent)
{
	if(isReadOnly[propName]) return 0;
	currentProperty = propName;

	QWidget *widget = new QWidget(parent);
	QHBoxLayout *layout = new QHBoxLayout(widget);
	layout->setMargin(0);

	fileEdit = new QLineEdit(value.toString(), widget);
	layout->addWidget(fileEdit);

	QToolButton *button = new QToolButton(widget);
	button->setText("...");
	button->setFixedWidth(20);
	layout->addWidget(button);

	connect(fileEdit, &QLineEdit::textChanged, this, &FilePropertyManager::filePathChanged);
	connect(button, &QToolButton::clicked, this, &FilePropertyManager::buttonClicked);

	widget->setFocusProxy(fileEdit);
	return widget;
}

bool FilePropertyManager::isValid(const QString &propName, const QVariant &value) const
{
	const QString file = value.toString();
	if(isRequired[propName] || !file.isEmpty())
		return QFile(file).exists();

	return PropertyManager::isValid(propName, value);
}

void FilePropertyManager::setFilter(const QString &propName, const QString &filter)
{
	filters[propName] = filter;
}

void FilePropertyManager::filePathChanged(QString newValue)
{
	emit modified(currentProperty, newValue);
}

void FilePropertyManager::buttonClicked()
{
	QString newFile = QFileDialog::getOpenFileName(fileEdit, QString(), fileEdit->text(), filters[currentProperty]);
	if(!newFile.isEmpty())
		fileEdit->setText(newFile);
}

QString EnumPropertyManager::valueToString(const QString &propName, const QVariant &value) const
{
	return enumNames[propName][value.toInt()];
}

QWidget *EnumPropertyManager::createEditor(const QString &propName, const QVariant &value, QWidget *parent)
{
	if(isReadOnly[propName]) return 0;
	currentProperty = propName;

	QComboBox *editor = new QComboBox(parent);
	editor->addItems(enumNames[propName]);
	editor->setCurrentIndex(value.toInt());

	connect(editor, SIGNAL(currentIndexChanged(int)), this, SLOT(currentValueChanged(int)));
	return editor;
}

void EnumPropertyManager::setEnumNames(const QString &propName, const QStringList &names)
{
	enumNames[propName] = names;
}

void EnumPropertyManager::currentValueChanged(int newValue)
{
	emit modified(currentProperty, newValue);
}

QString BoolPropertyManager::valueToString(const QString &, const QVariant &value) const
{
	return value.toBool() ? tr("Oui") : tr("Non");
}

QWidget *BoolPropertyManager::createEditor(const QString &propName, const QVariant &value, QWidget *parent)
{
	if(isReadOnly[propName]) return 0;
	currentProperty = propName;

	QWidget *widget = new QWidget(parent);
	QHBoxLayout *layout = new QHBoxLayout(widget);
	layout->setMargin(0);

	button = new QCheckBox(widget);
	button->setChecked(value.toBool());
	button->setText(valueToString(propName, value));
	layout->addWidget(button);

	connect(button, &QCheckBox::toggled, this, &BoolPropertyManager::buttonToogled);

	widget->setFocusProxy(button);
	return widget;
}

void BoolPropertyManager::buttonToogled(bool value)
{
	button->setText(valueToString(currentProperty, value));
	emit modified(currentProperty, value);
}

QString SizePropertyManager::valueToString(const QString &, const QVariant &value) const
{
	const QSize size = value.toSize();
	return QString("%1 x %2").arg(size.width()).arg(size.height());
}

QWidget *SizePropertyManager::createEditor(const QString &propName, const QVariant &value, QWidget *parent)
{
	if(isReadOnly[propName]) return 0;
	currentProperty = propName;

	const QSize size = value.toSize();

	QWidget *widget = new QWidget(parent);
	QHBoxLayout *layout = new QHBoxLayout(widget);
	layout->setMargin(0);

	width = new QSpinBox(widget);
	layout->addWidget(width);

	QLabel *separator = new QLabel("x", widget);
	separator->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
	layout->addWidget(separator);

	height = new QSpinBox(widget);
	layout->addWidget(height);

	lockRatio = new QToolButton(widget);
	lockRatio->setFixedWidth(20);
	lockRatio->setIcon(QIcon::fromTheme(QStringLiteral("emblem-locked")));
	lockRatio->setCheckable(true);
	layout->addWidget(lockRatio);

	if(minimums.contains(propName))
	{
		width->setMinimum(minimums[propName].width());
		height->setMinimum(minimums[propName].height());
	}

	if(maximums.contains(propName))
	{
		width->setMaximum(maximums[propName].width());
		height->setMaximum(maximums[propName].height());
	}
	else
	{
		width->setMaximum(INT_MAX);
		height->setMaximum(INT_MAX);
	}

	width->setValue(size.width());
	height->setValue(size.height());

	connect(width, SIGNAL(valueChanged(int)), this, SLOT(widthChanged(int)));
	connect(height, SIGNAL(valueChanged(int)), this, SLOT(heightChanged(int)));
	connect(lockRatio, &QPushButton::toggled, this, &SizePropertyManager::calculateRatio);

	widget->setFocusProxy(width);
	return widget;
}

void SizePropertyManager::setMinimum(const QString &propName, const QSize &min)
{
	minimums[propName] = min;
}

void SizePropertyManager::setMaximum(const QString &propName, const QSize &max)
{
	maximums[propName] = max;
}

void SizePropertyManager::calculateRatio()
{
	ratio = (double)width->value() / height->value();
}

void SizePropertyManager::widthChanged(int value)
{
	if(lockRatio->isChecked())
	{
		const int newHeight = (double)value / ratio;

		height->blockSignals(true);
		height->setValue(newHeight);
		height->blockSignals(false);

		if(newHeight > height->maximum())
			calculateRatio();
	}

	valuesChanged();
}

void SizePropertyManager::heightChanged(int value)
{
	if(lockRatio->isChecked())
	{
		const int newWidth = (double)value * ratio;

		width->blockSignals(true);
		width->setValue(newWidth);
		width->blockSignals(false);

		if(newWidth > width->maximum())
			calculateRatio();
	}

	valuesChanged();
}

void SizePropertyManager::valuesChanged()
{
	emit modified(currentProperty, QSize(width->value(), height->value()));
}

QString PointPropertyManager::valueToString(const QString &, const QVariant &value) const
{
	const QPoint point = value.toPoint();
	return QString("(%1, %2)").arg(point.x()).arg(point.y());
}

QWidget *PointPropertyManager::createEditor(const QString &propName, const QVariant &value, QWidget *parent)
{
	if(isReadOnly[propName]) return 0;
	currentProperty = propName;

	const QPoint point = value.toPoint();

	QWidget *widget = new QWidget(parent);
	QHBoxLayout *layout = new QHBoxLayout(widget);
	layout->setMargin(0);

	x = new QSpinBox(widget);
	layout->addWidget(x);

	QLabel *separator = new QLabel(", ", widget);
	separator->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
	layout->addWidget(separator);

	y = new QSpinBox(widget);
	layout->addWidget(y);

	if(minimums.contains(propName))
	{
		x->setMinimum(minimums[propName].x());
		y->setMinimum(minimums[propName].y());
	}

	if(maximums.contains(propName))
	{
		x->setMaximum(maximums[propName].x());
		y->setMaximum(maximums[propName].y());
	}
	else
	{
		x->setMaximum(INT_MAX);
		y->setMaximum(INT_MAX);
	}

	x->setValue(point.x());
	y->setValue(point.y());

	connect(x, SIGNAL(valueChanged(int)), this, SLOT(valuesChanged()));
	connect(y, SIGNAL(valueChanged(int)), this, SLOT(valuesChanged()));

	widget->setFocusProxy(x);
	return widget;
}

void PointPropertyManager::setMinimum(const QString &propName, const QPoint &min)
{
	minimums[propName] = min;
}

void PointPropertyManager::setMaximum(const QString &propName, const QPoint &max)
{
	maximums[propName] = max;
}

void PointPropertyManager::valuesChanged()
{
	emit modified(currentProperty, QPoint(x->value(), y->value()));
}

QString IntPropertyManager::valueToString(const QString &propName, const QVariant &value) const
{
	return QString("%1%2%3").arg(prefixes[propName], QString::number(value.toInt()), suffixes[propName]);
}

QWidget *IntPropertyManager::createEditor(const QString &propName, const QVariant &value, QWidget *parent)
{
	if(isReadOnly[propName]) return 0;
	currentProperty = propName;

	QSpinBox *editor = new QSpinBox(parent);

	if(minimums.contains(propName))
		editor->setMinimum(minimums[propName]);
	if(maximums.contains(propName))
		editor->setMaximum(maximums[propName]);
	else
		editor->setMaximum(INT_MAX);

	editor->setValue(value.toInt());

	editor->setPrefix(prefixes[propName]);
	editor->setSuffix(suffixes[propName]);

	connect(editor, SIGNAL(valueChanged(int)), this, SLOT(valueChanged(int)));

	return editor;

	return 0;
}

void IntPropertyManager::setMinimum(const QString &propName, const int min)
{
	minimums[propName] = min;
}

void IntPropertyManager::setMaximum(const QString &propName, const int max)
{
	maximums[propName] = max;
}

void IntPropertyManager::setPrefix(const QString &propName, const QString &prefix)
{
	prefixes[propName] = prefix;
}

void IntPropertyManager::setSuffix(const QString &propName, const QString &suffix)
{
	suffixes[propName] = suffix;
}

void IntPropertyManager::valueChanged(int newValue)
{
	emit modified(currentProperty, newValue);
}

QWidget *IntSliderPropertyManager::createEditor(const QString &propName, const QVariant &value, QWidget *parent)
{
	if(isReadOnly[propName]) return 0;
	currentProperty = propName;

	QSlider *editor = new QSlider(Qt::Horizontal, parent);

	if(minimums.contains(propName))
		editor->setMinimum(minimums[propName]);
	if(maximums.contains(propName))
		editor->setMaximum(maximums[propName]);

	editor->setValue(value.toInt());

	connect(editor, &QSlider::valueChanged, this, &IntSliderPropertyManager::valueChanged);

	return editor;
}

QString FontPropertyManager::valueToString(const QString &, const QVariant &value) const
{
	return fontToString(value.value<QFont>());
}

QIcon FontPropertyManager::valueToIcon(const QString &, const QVariant &value) const
{
	return QIcon(fontToPixmap(value.value<QFont>()));
}

QWidget *FontPropertyManager::createEditor(const QString &propName, const QVariant &value, QWidget *parent)
{
	if(isReadOnly[propName]) return 0;

	currentFont = value.value<QFont>();
	currentProperty = propName;

	QWidget *widget = new QWidget(parent);
	QHBoxLayout *layout = new QHBoxLayout(widget);
	layout->setMargin(0);

	fontPreview = new QLabel(widget);
	fontPreview->setPixmap(fontToPixmap(currentFont));
	layout->addWidget(fontPreview);

	fontLabel = new QLabel(fontToString(currentFont), widget);
	layout->addWidget(fontLabel);

	QToolButton *button = new QToolButton(widget);
	button->setText("...");
	button->setFixedWidth(20);
	layout->addWidget(button);

	connect(button, &QToolButton::clicked, this, &FontPropertyManager::buttonClicked);

	widget->setFocusProxy(button);
	return widget;
}

void FontPropertyManager::buttonClicked()
{
	QFontDialog dialog;
	dialog.setCurrentFont(currentFont);
	if(dialog.exec() == QDialog::Rejected)
		return;

	currentFont = dialog.currentFont();
	fontPreview->setPixmap(fontToPixmap(currentFont));
	fontLabel->setText(fontToString(currentFont));

	emit modified(currentProperty, currentFont);
}

QString FontPropertyManager::fontToString(const QFont &font) const
{
	return QString("[%1, %2]")
		.arg(font.family())
		.arg(font.pointSize());
}

QPixmap FontPropertyManager::fontToPixmap(const QFont &font) const
{
	QFont f = font;
	f.setPointSize(13);

	QImage img(16, 16, QImage::Format_ARGB32_Premultiplied);
	img.fill(0);

	QPainter painter(&img);
	painter.setRenderHint(QPainter::TextAntialiasing, true);
	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.setFont(f);

	QTextOption options;
	options.setAlignment(Qt::AlignCenter);

	painter.drawText(QRect(0, 0, 16, 16), QString(QLatin1Char('A')), options);
	return QPixmap::fromImage(img);
}

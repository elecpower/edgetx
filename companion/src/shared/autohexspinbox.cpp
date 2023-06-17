/*
 * Copyright (C) EdgeTX
 *
 * Based on code named
 *   opentx - https://github.com/opentx/opentx
 *   th9x - http://code.google.com/p/th9x
 *   er9x - http://code.google.com/p/er9x
 *   gruvin9x - http://code.google.com/p/gruvin9x
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "autohexspinbox.h"

AutoHexSpinBox::AutoHexSpinBox(QWidget * parent):
  AutoSpinBox(parent),
  m_validator(nullptr)
{
}

AutoHexSpinBox::~AutoHexSpinBox()
{
}

QValidator::State AutoHexSpinBox::validate(QString &text, int &pos) const
{
  if (m_validator)
    return m_validator->validate(text, pos);
  else
    return QValidator::Acceptable;
}

int AutoHexSpinBox::valueFromText(const QString &text) const
{
  return text.toInt(0, 16);
}

QString AutoHexSpinBox::textFromValue(int value) const
{
  return QString("%1").arg(value, params()->size(), 16, QChar('0')).toUpper();
}

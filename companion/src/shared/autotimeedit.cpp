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

#include "autotimeedit.h"

AutoTimeEdit::AutoTimeEdit(QWidget * parent):
  QTimeEdit(parent),
  AutoWidget(),
  m_field(nullptr)
{
  connect(this, &QTimeEdit::timeChanged, this, &AutoTimeEdit::onTimeChanged);
}

AutoTimeEdit::~AutoTimeEdit()
{
}

void AutoTimeEdit::setField(unsigned int & field, GenericPanel * panel, AutoWidgetParams * params)
{
  m_field = &field;
  init(panel, params);
}

void AutoTimeEdit::updateValue()
{
  if (m_field) {
    setLock(true);
    const int hour = *m_field / 3600;
    const int min = (*m_field - (hour * 3600)) / 60;
    const int sec = (*m_field - (hour * 3600)) % 60;
    setTime(QTime(hour, min, sec));
    setLock(false);
  }
}

void AutoTimeEdit::onTimeChanged(QTime time)
{
  if (m_field && !lock()) {
    *m_field = time.hour() * 3600 + time.minute() * 60 + time.second();
    emit currentDataChanged(*m_field);
    dataChanged();
  }
}

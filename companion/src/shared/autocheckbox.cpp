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

#include "autocheckbox.h"

AutoCheckBox::AutoCheckBox(QWidget * parent):
  QCheckBox(parent),
  AutoWidget(),
  m_field(nullptr)
{
  connect(this, &QCheckBox::toggled, this, &AutoCheckBox::onToggled);
}

AutoCheckBox::~AutoCheckBox()
{
}

// TODO: remove passing invert
void AutoCheckBox::setField(bool & field, GenericPanel * panel, bool invert, AutoWidgetParams * params)
{
  m_field = &field;
  init(panel, params);
  setInvert(invert);
}

// depreciated
void AutoCheckBox::setInvert(bool invert)
{
  params()->setInvert(invert);
  updateValue();
}

void AutoCheckBox::updateValue()
{
  if (m_field) {
    setLock(true);
    setChecked(params()->invert() ? !(*m_field) : *m_field);
    setLock(false);
  }
}

void AutoCheckBox::onToggled(bool checked)
{
  if (m_field && !lock()) {
    *m_field = params()->invert() ? !checked : checked;
    emit currentDataChanged(*m_field);
    dataChanged();
  }
}

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

#include "autospinbox.h"

AutoSpinBox::AutoSpinBox(QWidget * parent):
  QSpinBox(parent),
  AutoWidget(),
  m_field(nullptr)
{
  // ui signals to monitor and process
  connect(this, QOverload<int>::of(&QSpinBox::valueChanged), this, &AutoSpinBox::onValueChanged);

  // param signals to monitor and process
  connect(params(), &AutoWidgetParams::minChanged, [=] (float val) { setMinimum((int)val); updateValue(); });
  connect(params(), &AutoWidgetParams::maxChanged, [=] (float val) { setMaximum((int)val); updateValue(); });
  connect(params(), &AutoWidgetParams::stepChanged, [=] (float val) { setSingleStep((int)val); });
  connect(params(), &AutoWidgetParams::offsetChanged, [=] (float val) { updateValue(); });
  connect(params(), &AutoWidgetParams::prefixChanged, [=] (QString val) { setPrefix(val); });
  connect(params(), &AutoWidgetParams::suffixChanged, [=] (QString val) { setSuffix(val); });
  connect(params(), &AutoWidgetParams::intFuncsChanged, [=] () { updateValue(); });
  connect(params(), &AutoWidgetParams::bitMappingChanged, [=] () { updateValue(); });
}

AutoSpinBox::~AutoSpinBox()
{
}

void AutoSpinBox::setField(int & field, GenericPanel * panel, AutoWidgetParams * params)
{
  m_field = &field;
  init(panel, params);
}

void AutoSpinBox::setField(unsigned int & field, GenericPanel * panel, AutoWidgetParams * params)
{
  m_field = (int *)&field;
  init(panel, params);
}

void AutoSpinBox::updateValue()
{
  if (m_field) {
    setLock(true);
    int value = 0;

    if (params()->isBitMapped())
      value = getBitMappedValue(m_field) + params()->offset();
    else if (params()->hasIntFuncs())
      value = (params()->intReadFunc())(*m_field);
    else
      value = *m_field + params()->offset();

    setValue(value);
    setLock(false);
  }
}

void AutoSpinBox::onValueChanged(int value)
{
  if (m_field && !lock()) {
    if (params()->isBitMapped())
      setBitMappedValue(m_field, value - params()->offset());
    else if (params()->hasIntFuncs())
      *m_field = (params()->intWriteFunc())(value);
    else
      *m_field = value - params()->offset();

    emit currentDataChanged(*m_field);
    dataChanged();
  }
}

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

#include "autodoublespinbox.h"

#include <math.h>

AutoDoubleSpinBox::AutoDoubleSpinBox(QWidget * parent):
  QDoubleSpinBox(parent),
  AutoWidget(),
  m_field(nullptr)
{
  // ui signals to monitor and process
  connect(this, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &AutoDoubleSpinBox::onValueChanged);

  // param signals to monitor and process
  connect(params(), &AutoWidgetParams::precisionChanged, [=] (int val) { QDoubleSpinBox::setDecimals(val); updateValue(); });
  connect(params(), &AutoWidgetParams::minChanged, [=] (float val) { setMinimum(val); updateValue(); });
  connect(params(), &AutoWidgetParams::maxChanged, [=] (float val) { setMaximum(val); updateValue(); });
  connect(params(), &AutoWidgetParams::stepChanged, [=] (float val) { setSingleStep(val); });
  connect(params(), &AutoWidgetParams::offsetChanged, [=] (float val) { updateValue(); });
  connect(params(), &AutoWidgetParams::prefixChanged, [=] (QString val) { setPrefix(val); });
  connect(params(), &AutoWidgetParams::suffixChanged, [=] (QString val) { setSuffix(val); });
  connect(params(), &AutoWidgetParams::floatFuncsChanged, [=] () { updateValue(); });
}

AutoDoubleSpinBox::~AutoDoubleSpinBox()
{
}

void AutoDoubleSpinBox::setField(int & field, GenericPanel * panel, AutoWidgetParams * params)
{
  m_field = &field;
  init(panel, params);
}

void AutoDoubleSpinBox::setField(unsigned int & field, GenericPanel * panel, AutoWidgetParams * params)
{
  m_field = (int *)&field;
  init(panel, params);
}

// depreciated
void AutoDoubleSpinBox::setDecimals(int precision)
{
  params()->setPrecision(precision);
}

// depreciated
void AutoDoubleSpinBox::setOffset(int offset)
{
  params()->setOffset(offset);
}

void AutoDoubleSpinBox::updateValue()
{
  if (m_field) {
    setLock(true);
    const float val = params()->hasFloatFuncs() ? (params()->floatReadFunc())(*m_field) :
                                                   float(*m_field) / pow(10, params()->precision()) + params()->offset();
    setValue(val);
    setLock(false);
  }
}

void AutoDoubleSpinBox::onValueChanged(double value)
{
  if (m_field && !lock()) {
    const int val = params()->hasFloatFuncs() ? (params()->floatWriteFunc())(value) :
                                                 (int)((value - params()->offset()) * pow(10, params()->precision()));
    *m_field = val;
    emit currentDataChanged(*m_field);
    dataChanged();
  }
}

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
  connect(params(), &AutoWidgetParams::funcsChanged, [=] () {
    updateValue();
  });
  connect(params(), &AutoWidgetParams::maximumChanged, [=] (float val) {
    QDoubleSpinBox::setMaximum(val);
    updateValue();
  });
  connect(params(), &AutoWidgetParams::minimumChanged, [=] (float val) {
    QDoubleSpinBox::setMinimum(val);
    updateValue();
  });
  connect(params(), &AutoWidgetParams::offsetChanged, [=] (float val) {
    updateValue();
  });
  connect(params(), &AutoWidgetParams::precisionChanged, [=] (int val) {
    QDoubleSpinBox::setDecimals(val);
    updateValue();
  });
  connect(params(), &AutoWidgetParams::prefixChanged, [=] (QString val) {
    QDoubleSpinBox::setPrefix(val);
  });
  connect(params(), &AutoWidgetParams::stepChanged, [=] (float val) {
    QDoubleSpinBox::setSingleStep(val);
  });
  connect(params(), &AutoWidgetParams::suffixChanged, [=] (QString val) {
    QDoubleSpinBox::setSuffix(val);
  });
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

void AutoDoubleSpinBox::updateValue()
{
  if (m_field) {
    setLock(true);
    const double val = params()->hasDblFuncs() ? (params()->dblReadFunc())(*m_field) :
                                                 (double)(*m_field + params()->offset()) / params()->precision() ;
    setValue(val);
    setLock(false);
  }
}

void AutoDoubleSpinBox::onValueChanged(double value)
{
  if (m_field && !lock()) {
    const int val = params()->hasDblFuncs() ? (params()->dblWriteFunc())(value) :
                                              (int)(value * params()->precision()) - params()->offset());
    *m_field = val;
    emit currentDataChanged(*m_field);
    dataChanged();
  }
}

// depreciated
void AutoDoubleSpinBox::setOffset(int offset)
{
  params()->setOffset(offset);
}

// overriden Qt functions
void AutoDoubleSpinBox::setDecimals(int prec)
{
  params()->setPrecision(prec);
}

void AutoDoubleSpinBox::setMaximum(double max);
{
  params()->setMaximum(max);
}

void AutoDoubleSpinBox::setMinimum(double min);
{
  params()->setMinimum(min);
}

void AutoDoubleSpinBox::setPrefix(const QString &prefix);
{
  params()->setPrefix(prefix);
}

void AutoDoubleSpinBox::setRange(double min, double max);
{
  params()->setRange(min, max);
}

void AutoDoubleSpinBox::setSingleStep(double val);
{
  params()->setSingleStep(val);
}

void AutoDoubleSpinBox::setSuffix(const QString &suffix)
{
  params()->setSuffix(suffix);
}

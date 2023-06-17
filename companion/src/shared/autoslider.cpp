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

#include "autoslider.h"

AutoSlider::AutoSlider(QWidget * parent):
  QSlider(parent),
  AutoWidget(),
  m_field(nullptr)
{
  init();
}

AutoSlider::AutoSlider(Qt::Orientation orientation, QWidget * parent):
  QSlider(orientation, parent),
  AutoWidget()
{
  init();
}

AutoSlider::~AutoSlider()
{
}

void AutoSlider::init()
{
  // ui signals to monitor and process
  connect(this, &QSlider::valueChanged, this, &AutoSlider::onValueChanged);

  // param signals to monitor and process
  connect(params(), &AutoWidgetParams::minChanged, [=] (float val) { setMinimum(val); updateValue(); });
  connect(params(), &AutoWidgetParams::maxChanged, [=] (float val) { setMaximum(val); updateValue(); });
  connect(params(), &AutoWidgetParams::offsetChanged, [=] (float val) { updateValue(); });
  connect(params(), &AutoWidgetParams::intFuncsChanged, [=] () { updateValue(); });
}

// TODO: remove passing min and max
void AutoSlider::setField(int & field, int min, int max, GenericPanel * panel, AutoWidgetParams * params)
{
  m_field = &field;
  initField(min, max, panel, params);
}

// TODO: remove passing min and max
void AutoSlider::setField(unsigned int & field, int min, int max, GenericPanel * panel, AutoWidgetParams * params)
{
  m_field = (int *)&field;
  initField(min, max, panel, params);
}

void AutoSlider::setTick(int interval, QSlider::TickPosition position)
{
  setTickInterval(interval);
  setTickPosition(position);
}

// TODO: remove passing min and max
void AutoSlider::initField(int min, int max, GenericPanel * panel, AutoWidgetParams * params)
{
  AutoWidget::init(panel, params);
  this->params()->setMin(min);
  this->params()->setMax(max);
}

void AutoSlider::updateValue()
{
  if (m_field) {
    setLock(true);
    const int value = params()->hasIntFuncs() ? (params()->intReadFunc())(*m_field) :
                                                *m_field + params()->offset();
    setValue(value);
    setLock(false);
  }
}

void AutoSlider::onValueChanged(int value)
{
  if (m_field && !lock()) {
    const int val = params()->hasIntFuncs() ? (params()->intWriteFunc())(value) :
                                              value - params()->offset();
    *m_field = val;
    emit currentDataChanged(*m_field);
    dataChanged();
  }
}

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

#include "autowidget.h"
#include "genericpanel.h"

AutoWidget::AutoWidget():
  m_panel(nullptr),
  m_params(new AutoWidgetParams()),
  m_lock(false)
{
}

AutoWidget::~AutoWidget()
{
}

bool AutoWidget::lock()
{
  return panelLock() | m_lock;
}

void AutoWidget::setLock(bool lock)
{
  m_lock = lock;
}

bool AutoWidget::panelLock()
{
  if (m_panel)
    return m_panel->lock;
  else
    return false;
}

void AutoWidget::init(GenericPanel * panel, AutoWidgetParams * params)
{
  m_panel = panel;
  setParams(params);
}

void AutoWidget::setParams(AutoWidgetParams * params)
{
  if (params)
    *m_params = *params; // deep copy as no control over life of source
  else if (m_params)
    m_params->clear();
}

void AutoWidget::dataChanged()
{
  if (m_panel)
    emit m_panel->modified();
}

int AutoWidget::getBitMappedValue(int * field)
{
  return (*field >> bitShift()) & bitMask();
}

void AutoWidget::setBitMappedValue(int * field, const int value)
{
  int fieldmask = bitMask() << bitShift();
  *field = (*field & ~fieldmask) | (value << bitShift());
}

int AutoWidget::bitMask()
{
  int mask = -1;
  mask = mask << m_params->bitsCount();
  mask = ~mask;
  return mask;
}

int AutoWidget::bitShift()
{
  return m_params->bitsShift() + m_params->bitsCount() * m_params->bitsIndex();
}

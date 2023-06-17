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

#include "autowidgetparams.h"

AutoWidgetParams::AutoWidgetParams()
{
  clear();
}

AutoWidgetParams::~AutoWidgetParams()
{
}

void AutoWidgetParams::clear()
{
  m_size = 0;
  m_precision = 0;
  m_min = 0;
  m_max = 0;
  m_step = 1.0;
  m_offset = 0;
  m_invert = false;
  m_inputMask = QString();
  m_prefix = QString();
  m_suffix = QString();
  m_type = AWPT_DEFAULT;
  m_u.m_bitmapping = { 0, 0, 0 };
}

// perform a deep copy and emit changed signals for ui widgets to react
AutoWidgetParams& AutoWidgetParams::operator=(const AutoWidgetParams& rhs)
{
  setSize(rhs.m_size);
  setPrecision(rhs.m_precision);
  setMin(rhs.m_min);
  setMax(rhs.m_max);
  setStep(rhs.m_step);
  setOffset(rhs.m_offset);
  setInvert(rhs.m_invert);
  setInputMask(rhs.m_inputMask);
  setPrefix(rhs.m_prefix);
  setSuffix(rhs.m_suffix);

  switch ((int)rhs.m_type) {
    case AWPT_BITMAPPED:
      setBitMapping(rhs.m_u.m_bitmapping.m_bits, rhs.m_u.m_bitmapping.m_shift, rhs.m_u.m_bitmapping.m_index);
      break;
    case AWPT_FLOAT_FUNCS:
      setCustomFuncs(rhs.m_u.m_awp_float.m_read, rhs.m_u.m_awp_float.m_write);
      break;
    case AWPT_INT_FUNCS:
      setCustomFuncs(rhs.m_u.m_awp_int.m_read, rhs.m_u.m_awp_int.m_write);
      break;
    default:
      break;
  }

  return *this;
}

/*
  Custom set functions
*/
void AutoWidgetParams::setMin(const float & val)
{
  if (val != m_min) {
    if (val > m_max)
      m_min = m_max;
    else
      m_min = val;

    emit minChanged(m_min);
  }
}

void AutoWidgetParams::setMax(const float & val)
{
  if (val != m_max) {
    if (val < m_min)
      m_max = m_min;
    else
      m_max = val;

    emit maxChanged(m_max);
  }
}

void AutoWidgetParams::setBitMapping(const int bits, const int shift, const int index)
{
  if (bits) {
    m_type = AWPT_BITMAPPED;
    m_u.m_bitmapping.m_shift = shift;
    m_u.m_bitmapping.m_index = index;
    m_u.m_bitmapping.m_bits = bits;
    emit bitMappingChanged();
  }
}

void AutoWidgetParams::setCustomFuncs(awp_float_read_func read, awp_float_write_func write)
{
  if (read && write) {
    m_type = AWPT_FLOAT_FUNCS;
    m_u.m_awp_float.m_read = read;
    m_u.m_awp_float.m_write = write;
    emit floatFuncsChanged();
  }
}

void AutoWidgetParams::setCustomFuncs(awp_int_read_func read, awp_int_write_func write)
{
  if (read && write) {
    m_type = AWPT_INT_FUNCS;
    m_u.m_awp_int.m_read = read;
    m_u.m_awp_int.m_write = write;
    emit intFuncsChanged();
  }
}

/*
  Helper functions
*/

void AutoWidgetParams::setRange(const float & min, const float & max)
{
  if (min < max) {
    if (min < m_max) {
      setMin(min);
      setMax(max);
    }
    else {
      setMax(max);
      setMin(min);
    }
  }
}

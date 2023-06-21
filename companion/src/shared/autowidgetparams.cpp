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
  m_precision = pow(10, 0);
  m_minimum = 0;
  m_maximum = 0;
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
  setMinimum(rhs.m_minimum);
  setMaximum(rhs.m_maximum);
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
    case AWPT_DBL_FUNCS:
      setCustomFuncs(rhs.m_u.m_awp_dbl.m_read, rhs.m_u.m_awp_dbl.m_write);
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
void AutoWidgetParams::setMinimum(const double & val)
{
  if (val != m_minimum) {
    if (val > m_maximum)
      m_minimum = m_maximum;
    else
      m_minimum = val;

    emit minimumChanged(m_minimum);
  }
}

void AutoWidgetParams::setMaximum(const double & val)
{
  if (val != m_maximum) {
    if (val < m_minimum)
      m_maximum = m_minimum;
    else
      m_maximum = val;

    emit maximumChanged(m_maximum);
  }
}

void AutoWidgetParams::setPrecision(int prec)
{
  m_precision = pow(10, prec);
  emit precisionChanged(prec);
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

void AutoWidgetParams::setCustomFuncs(awp_dbl_read_func read, awp_dbl_write_func write)
{
  if (read && write) {
    m_type = AWPT_DBL_FUNCS;
    m_u.m_awp_dbl.m_read = read;
    m_u.m_awp_dbl.m_write = write;
    emit funcsChanged();
  }
}

void AutoWidgetParams::setCustomFuncs(awp_int_read_func read, awp_int_write_func write)
{
  if (read && write) {
    m_type = AWPT_INT_FUNCS;
    m_u.m_awp_int.m_read = read;
    m_u.m_awp_int.m_write = write;
    emit funcsChanged();
  }
}

/*
  Helper functions
*/

void AutoWidgetParams::setRange(double min, double max)
{
  if (min < max) {
    if (min < m_max) {
      setMinimum(min);
      setMaximum(max);
    }
    else {
      setMaximum(max);
      setMinimum(min);
    }
  }
}

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

#pragma once

#include <QtCore>

class AutoWidgetParams : public QObject
{
  Q_OBJECT

  public:
    // Note: internal data structs store numbers as int/unsigned int and
    //       use precision/decimals to convert to double for ui
    typedef double (*awp_dbl_read_func)(int);
    typedef int (*awp_dbl_write_func)(double);

    typedef int (*awp_int_read_func)(int);
    typedef int (*awp_int_write_func)(int);

    enum AutoWidgetParamsType {
      AWPT_DEFAULT,
      AWPT_BITMAPPED,
      AWPT_DBL_FUNCS,
      AWPT_INT_FUNCS,
      AWPT_LAST
    };

    explicit AutoWidgetParams();
    virtual ~AutoWidgetParams();

    AutoWidgetParams& operator=(const AutoWidgetParams& rhs);

    void clear();

    void setRange(double min, double max);

    inline bool isBitMapped() { return m_type == AWPT_BITMAPPED && m_u.m_bitmapping.m_bits; }
    inline bool hasDblFuncs() { return (m_type == AWPT_DBL_FUNCS && m_u.m_awp_dbl.m_read && m_u.m_awp_dbl.m_write); }
    inline bool hasIntFuncs() { return (m_type == AWPT_INT_FUNCS && m_u.m_awp_int.m_read && m_u.m_awp_int.m_write); }

    void setBitMapping(const int bits, const int shift = 0, const int index = 0);
    inline int bitsCount() const { return m_type == AWPT_BITMAPPED ? m_u.m_bitmapping.m_bits : 0; }
    inline int bitsShift() const { return m_type == AWPT_BITMAPPED ? m_u.m_bitmapping.m_shift : 0; }
    inline int bitsIndex() const { return m_type == AWPT_BITMAPPED ? m_u.m_bitmapping.m_index : 0; }

    void setCustomFuncs(awp_dbl_read_func read, awp_dbl_write_func write);
    inline awp_dbl_read_func dblReadFunc() const { return m_type == AWPT_DBL_FUNCS ? m_u.m_awp_dbl.m_read : nullptr; }
    inline awp_dbl_write_func dblWriteFunc() const { return m_type == AWPT_DBL_FUNCS ? m_u.m_awp_dbl.m_write : nullptr; }

    void setCustomFuncs(awp_int_read_func read, awp_int_write_func write);
    inline awp_int_read_func intReadFunc() const { return m_type == AWPT_INT_FUNCS ? m_u.m_awp_int.m_read : nullptr; }
    inline awp_int_write_func intWriteFunc() const { return m_type == AWPT_INT_FUNCS ? m_u.m_awp_int.m_write : nullptr; }

#define PARAM_COMMON(TYPE, NAME)                  \
  public:                                         \
    inline TYPE NAME() const { return m_##NAME; } \
  Q_SIGNALS:                                      \
    void NAME##Changed(const TYPE & val);         \
  private:                                        \
    TYPE m_##NAME;

#define PARAM_DFLT(TYPE, NAME, NAME1)             \
    PARAM_COMMON(TYPE, NAME)                      \
  public Q_SLOTS:                                 \
    void set##NAME1(const TYPE &val) {            \
      if (val == m_##NAME) return;                \
      m_##NAME = val;                             \
      emit NAME##Changed(val);                    \
    }

#define PARAM_CUST(TYPE, NAME, NAME1)             \
    PARAM_COMMON(TYPE, NAME)                      \
  public:                                         \
    void set##NAME1(const TYPE &val);

    PARAM_DFLT(int, size, Size)               // used for ui display/edit size
    PARAM_CUST(int, precision, Precision)     // converts internal to/from ui value and is applied to accepting ui widgets
    PARAM_CUST(double, minimum, Minimum)      // ui min value
    PARAM_CUST(double, maximum, Maximum)      // ui max value
    PARAM_DFLT(double, step, Step)            // used to increment/decrement ui value
    PARAM_DFLT(int, offset, Offset)           // adjusts internal to/from ui value eg internal 2 + offset 5 = ui 7 - offset 5 = internal 2
    PARAM_DFLT(bool, invert, Invert)          // used for flipping booleans
    PARAM_DFLT(QString, inputMask, InputMask) // used for ui display/edit for fields supporting parameter eg QLineEdit
    PARAM_DFLT(QString, prefix, Prefix)       // ui display
    PARAM_DFLT(QString, suffix, Suffix)       // ui display

  signals:
    void bitMappingChanged();
    void funcsChanged();

  private:

    AutoWidgetParamsType m_type;              // indicates the union struct in use especially important in struct copy
    union {
      // use for compound int/unsigned int fields
      // value calculation is m_shift + (m_index x m_bits) for m_bits
      // depending on the widget, other parameters (eg invert, offset) are applied to extacted bits to/from ui value
      struct {
        int m_shift;   // anchor
        int m_index;   // combined with m_bits for secondary shift
        int m_bits;    // number of bits holding the value
      } m_bitmapping;
      // should none of the standard conversions for internal to/from ui value be suitable then
      // custom function pointers can be passed
      // all conversions between internal and ui are assumed to be handled by the functions
      struct {
        awp_dbl_read_func m_read;
        awp_dbl_write_func m_write;
      } m_awp_dbl;
      struct {
        awp_int_read_func m_read;
        awp_int_write_func m_write;
      } m_awp_int;
    } m_u;
};

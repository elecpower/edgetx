/*
 * Copyright (C) OpenTX
 *
 * Based on code named
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
#include <string>
#include <vector>

struct StringTagMapping {
  std::string name;
  std::string tag;
  unsigned int seq;

  StringTagMapping() = default;
  StringTagMapping(const char* name) :
      name(name), tag(name), seq(0)
  {
  }
  StringTagMapping(const std::string& name) :
      name(name), tag(name), seq(0)
  {
  }
  StringTagMapping(const char* name, const char* tag, const unsigned int seq = 0) :
      name(name), tag(tag), seq(seq)
  {
  }
  StringTagMapping(const std::string& name, const std::string& tag, const unsigned int seq = 0) :
      name(name), tag(tag), seq(seq)
  {
  }
};

typedef std::vector<StringTagMapping> StringTagMappingTable;

#define STRINGTAGMAPPINGFUNCS_HELPER(tbl, name)                   \
    inline int name##Index (const char * tag)                     \
    {                                                             \
      return DataHelpers::getStringTagMappingIndex(tbl, tag);     \
    }                                                             \
                                                                  \
    inline std::string name##Tag (unsigned int index)             \
    {                                                             \
      return DataHelpers::getStringTagMappingTag(tbl, index);     \
    }

#define STRINGTAGMAPPINGFUNCS_ADC_HELPER(tbl, tbladc, name)       \
    STRINGTAGMAPPINGFUNCS_HELPER(tbl, name)                       \
                                                                  \
    inline int name##IndexADC (const char * tag)                  \
    {                                                             \
      return DataHelpers::getStringTagMappingIndex(tbladc, tag);  \
    }                                                             \
                                                                  \
    inline std::string name##TagADC (unsigned int index)          \
    {                                                             \
      return DataHelpers::getStringTagMappingTag(tbladc, index);  \
    }                                                             \
                                                                  \
    inline int name##SeqADC (unsigned int index)                  \
    {                                                             \
      return DataHelpers::getStringTagMappingSeq(tbladc, index);  \
    }                                                             \
                                                                  \
    inline std::string name##SeqTagADC (unsigned int seq)         \
    {                                                             \
      return DataHelpers::getStringSeqMappingTag(tbladc, seq);    \
    }

#define STRINGTAGMAPPINGFUNCS(tbl, name)  STRINGTAGMAPPINGFUNCS_HELPER(tbl, get##name)
#define STRINGTAGMAPPINGFUNCS_ADC(tbl, tbladc, name)  STRINGTAGMAPPINGFUNCS_ADC_HELPER(tbl, tbladc, get##name)

class FieldRange
{
  public:
    FieldRange():
      decimals(0),
      min(0.0),
      max(0.0),
      step(1.0),
      offset(0.0),
      prefix(""),
      unit("")
    {
    }

    float getValue(int value) { return float(value) * step; }

    int decimals;
    double min;
    double max;
    double step;
    double offset;
    QString prefix;
    QString unit;
};


constexpr unsigned int TIMESTR_MASK_HRSMINS   { 1 << 1 };
constexpr unsigned int TIMESTR_MASK_ZEROHRS   { 1 << 2 };
constexpr unsigned int TIMESTR_MASK_PADSIGN   { 1 << 3 };

namespace DataHelpers
{
  enum BoolFormat {
    BOOL_FMT_ENABLEDISABLE,
    BOOL_FMT_ONOFF,
    BOOL_FMT_TRUEFALSE,
    BOOL_FMT_YN,
    BOOL_FMT_YESNO
  };

  QString boolToString(const bool value, const BoolFormat format);
  QString getElementName(const QString & prefix, const unsigned int index, const char * name = 0, const bool padding = false);
  QString timeToString(const int value, const unsigned int mask);
  int getStringTagMappingIndex(const StringTagMappingTable& lut, const char * tag);
  std::string getStringTagMappingTag(const StringTagMappingTable& lut, unsigned int index);
  int getStringTagMappingSeq(const StringTagMappingTable& lut, unsigned int index);
  std::string getStringSeqMappingTag(const StringTagMappingTable& lut, unsigned int seq);
  std::string getStringNameMappingTag(const StringTagMappingTable& lut, const char * name);
}

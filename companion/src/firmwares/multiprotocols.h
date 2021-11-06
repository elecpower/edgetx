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

//#include <vector>
#include <QtCore>
//#include "moduledata.h"
#include "datahelpers.h"

// identiying names of static abstract item models
constexpr char AIM_MULTI_PROTOCOL[]        {"multi.protocol"};
constexpr char AIM_MULTI_SUBTYPE[]         {"multi.subtype"};
constexpr char AIM_MULTI_OPTIONVALUE[]     {"multi.optionvalue"};

class AbstractStaticItemModel;

class Multiprotocols
{
    Q_DECLARE_TR_FUNCTIONS(Multiprotocols)

  public:
    enum ValueUiWidget
    {
      VALUE_UI_WIDGET_NONE,
      VALUE_UI_WIDGET_SPINBOX,
      VALUE_UI_WIDGET_COMBOBOX
    };


      /*
      struct MultiProtocolDefinition {
      const int protocol;
      const QString protocolString;
      const bool hasFailsafe;
      const QStringList subTypeStrings;
      const int optionType;

      unsigned int numSubTypes() const;

      int getOptionMin() const;

      int getOptionMax() const;

      MultiProtocolDefinition(const MultiProtocolsDIY::mm_protocol_definition &diy);
    };

    Multiprotocols(std::initializer_list<MultiProtocolsDIY::mm_protocol_definition> l);
    */

    //const MultiProtocolDefinition &getProtocol(int protocol) const;
    static QString getDefinitionVersion();
    static QString protocolToString(int protocol);
    static QString subTypeToString(int protocol, unsigned subType);
    static int getMaxChannelCount(int protocol, unsigned subType);
    static QString optionTypeToString(int protocol, unsigned subType);
    static QString optionValueToString(int protocol, unsigned subType, int optionValue);
    static int optionTypeMin(int protocol, unsigned subType);
    static int optionTypeMax(int protocol, unsigned subType);
    static int optionTypeDefault(int protocol, unsigned subType);
    static FieldRange optionTypeRange(int protocol, unsigned subType);
    static int optionTypeValueUiWidget(int protocol, unsigned subType);
    static AbstractStaticItemModel *protocolItemModel();
    static AbstractStaticItemModel *subTypeItemModel();

  private:

    //std::vector<MultiProtocolDefinition> protocols;

};

//extern const Multiprotocols multiProtocols;

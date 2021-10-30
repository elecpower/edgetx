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

#include <vector>
#include <QtCore>
#include "moduledata.h"

class Multiprotocols
{
    Q_DECLARE_TR_FUNCTIONS(Multiprotocols)

  public:

//  Protocol number, Protocol String, Sub_protocol strings, Number of sub_protocols, Option type, Failsafe, ChMap, RF switch, Init, Callback
    struct radio_mm_definition {
      int protocol;
      QString protocolStr;
      QStringList subProtocols;
      unsigned int maxSubtype;
      QString optionStr;
      bool hasFailsafe;
      int channelMap;
      int rfSwitch;
      int funcInit;
      int funcCallback;
    };

    struct MultiProtocolDefinition {
      const int protocol;
      const QString protocolStr;
      const bool hasFailsafe;
      const QStringList subTypeStrings;
      const QString optionStr;

      unsigned int numSubTypes() const;

      int getOptionMin() const;

      int getOptionMax() const;

      MultiProtocolDefinition(const radio_mm_definition &rd) :
        // do not use all the fields required for the radio so drop them
        protocol(rd.protocol),
        protocolStr(rd.protocolStr),
        hasFailsafe(rd.hasFailsafe),
        subTypeStrings(rd.subProtocols),
        optionStr(rd.optionStr)
      {
        Q_ASSERT(rd.maxSubtype + 1 == (unsigned int) rd.subProtocols.length());
      }
    };

    Multiprotocols(std::initializer_list<radio_mm_definition> l)
    {
      for (radio_mm_definition rd: l)
        protocols.push_back(MultiProtocolDefinition(rd));
    }

    const MultiProtocolDefinition &getProtocol(int protocol) const;
    static QString protocolToString(int protocol, bool custom = false);
    static QString subTypeToString(int protocol, unsigned subType);
    static int getMaxChannelCount(int protocol, unsigned subType);

  private:

    std::vector<MultiProtocolDefinition> protocols;

};

extern const Multiprotocols multiProtocols;

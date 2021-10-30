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

// generated from https://github.com/pascallanger/DIY-Multiprotocol-TX-Module/Multiprotocol/Multiprotocol.h
#define MODULE_MULTI_VERSION_MAJOR		1
#define MODULE_MULTI_VERSION_MINOR		3
#define MODULE_MULTI_VERSION_REVISION	3
#define MODULE_MULTI_VERSION_PATCH_LEVEL	4

// generated from https://github.com/pascallanger/DIY-Multiprotocol-TX-Module/Multiprotocol/Multiprotocol.h
enum MultiModuleRFProtocols {
  MODULE_SUBTYPE_MULTI_FLYSKY = 0,
  MODULE_SUBTYPE_MULTI_FIRST = MODULE_SUBTYPE_MULTI_FLYSKY,
  MODULE_SUBTYPE_MULTI_HUBSAN,
  MODULE_SUBTYPE_MULTI_FRSKY,
  MODULE_SUBTYPE_MULTI_HISKY,
  MODULE_SUBTYPE_MULTI_V2X2,
  MODULE_SUBTYPE_MULTI_DSM2,
  MODULE_SUBTYPE_MULTI_DEVO,
  MODULE_SUBTYPE_MULTI_YD717,
  MODULE_SUBTYPE_MULTI_KN,
  MODULE_SUBTYPE_MULTI_SYMAX,
  MODULE_SUBTYPE_MULTI_SLT,
  MODULE_SUBTYPE_MULTI_CX10,
  MODULE_SUBTYPE_MULTI_CG023,
  MODULE_SUBTYPE_MULTI_BAYANG,
  MODULE_SUBTYPE_MULTI_ESky,
  MODULE_SUBTYPE_MULTI_MT99XX,
  MODULE_SUBTYPE_MULTI_MJXQ,
  MODULE_SUBTYPE_MULTI_SHENQI,
  MODULE_SUBTYPE_MULTI_FY326,
  MODULE_SUBTYPE_MULTI_FUTABA,
  MODULE_SUBTYPE_MULTI_J6PRO,
  MODULE_SUBTYPE_MULTI_FQ777,
  MODULE_SUBTYPE_MULTI_ASSAN,
  MODULE_SUBTYPE_MULTI_HONTAI,
  MODULE_SUBTYPE_MULTI_OLRS,
  MODULE_SUBTYPE_MULTI_FS_AFHDS2A,
  MODULE_SUBTYPE_MULTI_Q2X2,
  MODULE_SUBTYPE_MULTI_WK_2X01,
  MODULE_SUBTYPE_MULTI_Q303,
  MODULE_SUBTYPE_MULTI_GW008,
  MODULE_SUBTYPE_MULTI_DM002,
  MODULE_SUBTYPE_MULTI_CABELL,
  MODULE_SUBTYPE_MULTI_ESKY150,
  MODULE_SUBTYPE_MULTI_H83D,
  MODULE_SUBTYPE_MULTI_CORONA,
  MODULE_SUBTYPE_MULTI_CFLIE,
  MODULE_SUBTYPE_MULTI_HITEC,
  MODULE_SUBTYPE_MULTI_WFLY,
  MODULE_SUBTYPE_MULTI_BUGS,
  MODULE_SUBTYPE_MULTI_BUGS_MINI,
  MODULE_SUBTYPE_MULTI_TRAXXAS,
  MODULE_SUBTYPE_MULTI_NCC1701,
  MODULE_SUBTYPE_MULTI_E01X,
  MODULE_SUBTYPE_MULTI_V911S,
  MODULE_SUBTYPE_MULTI_GD00X,
  MODULE_SUBTYPE_MULTI_V761,
  MODULE_SUBTYPE_MULTI_KF606,
  MODULE_SUBTYPE_MULTI_REDPINE,
  MODULE_SUBTYPE_MULTI_POTENSIC,
  MODULE_SUBTYPE_MULTI_ZSX,
  MODULE_SUBTYPE_MULTI_HEIGHT,
  MODULE_SUBTYPE_MULTI_SCANNER,
  MODULE_SUBTYPE_MULTI_FRSKYX_RX,
  MODULE_SUBTYPE_MULTI_AFHDS2A_RX,
  MODULE_SUBTYPE_MULTI_HOTT,
  MODULE_SUBTYPE_MULTI_FX816,
  MODULE_SUBTYPE_MULTI_BAYANG_RX,
  MODULE_SUBTYPE_MULTI_PELIKAN,
  MODULE_SUBTYPE_MULTI_TIGER,
  MODULE_SUBTYPE_MULTI_XK,
  MODULE_SUBTYPE_MULTI_XN297DUMP,
  MODULE_SUBTYPE_MULTI_FRSKYX2,
  MODULE_SUBTYPE_MULTI_FRSKY_R9,
  MODULE_SUBTYPE_MULTI_PROPEL,
  MODULE_SUBTYPE_MULTI_FRSKYL,
  MODULE_SUBTYPE_MULTI_SKYARTEC,
  MODULE_SUBTYPE_MULTI_ESKY150V2,
  MODULE_SUBTYPE_MULTI_DSM_RX,
  MODULE_SUBTYPE_MULTI_JJRC345,
  MODULE_SUBTYPE_MULTI_Q90C,
  MODULE_SUBTYPE_MULTI_KYOSHO,
  MODULE_SUBTYPE_MULTI_RLINK,
  MODULE_SUBTYPE_MULTI_ELRS,
  MODULE_SUBTYPE_MULTI_REALACC,
  MODULE_SUBTYPE_MULTI_OMP,
  MODULE_SUBTYPE_MULTI_MLINK,
  MODULE_SUBTYPE_MULTI_WFLY2,
  MODULE_SUBTYPE_MULTI_E016HV2,
  MODULE_SUBTYPE_MULTI_COUNT,
  MODULE_SUBTYPE_MULTI_LAST = MODULE_SUBTYPE_MULTI_COUNT - 1
};

class Multiprotocols
{
    Q_DECLARE_TR_FUNCTIONS(Multiprotocols)

  public:

    struct radio_mm_definition {
      int protocol;
      unsigned int maxSubtype;
      bool hasFailsafe;
      QStringList protocols;
      QString optionsstr;
    };

    struct MultiProtocolDefinition {
      const int protocol;
      const bool hasFailsafe;
      const QStringList subTypeStrings;
      const QString optionsstr;

      unsigned int numSubTypes() const
      {
        return protocol > MODULE_SUBTYPE_MULTI_LAST ? 8 : (unsigned int) subTypeStrings.length();
      }

      int getOptionMin() const;

      int getOptionMax() const;

      MultiProtocolDefinition(const radio_mm_definition &rd) :
        protocol(rd.protocol),
        hasFailsafe(rd.hasFailsafe),
        subTypeStrings(rd.protocols),
        optionsstr(rd.optionsstr)
      {
        Q_ASSERT(rd.maxSubtype + 1 == (unsigned int) rd.protocols.length());
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

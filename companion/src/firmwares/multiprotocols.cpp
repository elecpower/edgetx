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

#include <QObject>

#include "multiprotocols.h"
#include "radiodata.h"
#include "multiprotocols_diy.h"

//  TODO how is this handled in the DIY code??????
#define STR_MULTI_SUBTYPE                    QT_TRANSLATE_NOOP("Multiprotocols", "Subtype")
#define STR_MULTI_VIDFREQ                    QT_TRANSLATE_NOOP("Multiprotocols", "Video TX frequency")
#define STR_MULTI_RFTUNE                     QT_TRANSLATE_NOOP("Multiprotocols", "CC2500 frequency fine tune")
#define STR_MULTI_RFPOWER                    QT_TRANSLATE_NOOP("Multiprotocols", "RF power")
#define STR_MULTI_TELEMETRY                  QT_TRANSLATE_NOOP("Multiprotocols", "Telemetry")
#define STR_MULTI_SERVOFREQ                  QT_TRANSLATE_NOOP("Multiprotocols", "Servo output frequency")
#define STR_MULTI_OPTION                     QT_TRANSLATE_NOOP("Multiprotocols", "Option value")
#define STR_MULTI_FIXEDID                    QT_TRANSLATE_NOOP("Multiprotocols", "Fixed ID value")
#define STR_MULTI_DEFAULT                    QT_TRANSLATE_NOOP("Multiprotocols", "DEFAULT")

Multiprotocols::MultiProtocolDefinition::MultiProtocolDefinition(const MultiProtocolsDIY::mm_protocol_definition &diy) :
  // we do not use all the fields
  protocol(diy.protocol),
  protocolStr(diy.ProtoString),
  hasFailsafe(diy.failsafe),
  subTypeStrings(diy.subProtocols),
  optionStr(diy.optionStr)
{
  Q_ASSERT(diy.maxSubtype + 1 == (unsigned int) diy.subProtocols.length());
}

Multiprotocols::Multiprotocols(std::initializer_list<MultiProtocolsDIY::mm_protocol_definition> l)
{
  for (MultiProtocolsDIY::mm_protocol_definition diy: l)
    protocols.push_back(MultiProtocolDefinition(diy));
}

int Multiprotocols::MultiProtocolDefinition::getOptionMin() const {
  if (optionsstr == STR_MULTI_RFPOWER)
    return -1;
  else if (optionsstr == STR_MULTI_SERVOFREQ)
    return 0;
  else
    return -128;
}

int Multiprotocols::MultiProtocolDefinition::getOptionMax() const {
  if (optionsstr == STR_MULTI_RFPOWER)
    return 7;
  else if (optionsstr == STR_MULTI_SERVOFREQ)
    return 70;
  else
    return 127;
}

unsigned int Multiprotocols::MultiProtocolDefinition::numSubTypes() const
{
  return protocol > MODULE_SUBTYPE_MULTI_LAST ? 8 : (unsigned int) subTypeStrings.length();
}


const Multiprotocols::MultiProtocolDefinition & Multiprotocols::getProtocol(int protocol) const {
  for (const Multiprotocols::MultiProtocolDefinition & proto: protocols)
    if (proto.protocol == protocol)
      return proto;

  // Protocol not found, return the default (last) proto
  Q_ASSERT(protocols.rbegin()->protocol == 0xfe);
  return *protocols.rbegin();
}

// static
QString Multiprotocols::protocolToString(int protocol)
{
  for (const Multiprotocols::MultiProtocolDefinition & proto: protocols)
    if (proto.protocol == protocol)
      return proto.protocolStr;

  return CPN_STR_UNKNOWN_ITEM;
}

// static
QString Multiprotocols::subTypeToString(int protocol, unsigned subType)
{
  if (subType < multiProtocols.getProtocol(protocol).numSubTypes) {
    const unsigned int sub_len = multiProtocols.getProtocol(protocol).subTypeStrings[0];
    if (sub_len > 0)
      return multiProtocols.getProtocol(protocol).subTypeStrings[subType * sub_len]);
  }

  return CPN_STR_UNKNOWN_ITEM;
}

// static
int Multiprotocols::getMaxChannelCount(int protocol, unsigned subType)
{
  if (subType == MODULE_SUBTYPE_MULTI_DSM2)
    return 12;
  else
    return 16;
}

// static
QString Multiprotocols::getDefinitionVersion()
{
  return QString("%1.%2.%3.%4").arg(VERSION_MAJOR).arg(VERSION_MINOR).arg(VERSION_REVISION).arg(VERSION_PATCH_LEVEL);
}




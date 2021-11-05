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

//#include <QObject>

#include "multiprotocols.h"
//#include "radiodata.h"
#include "multiprotocols_diy.h"

/*
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
  for (const Multiprotocols::MultiProtocolDefinition & proto: protocols)  //  convert from vector to array
    if (proto.protocol == protocol)   // break when proto.protocol = 0xFF
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
*/

const mm_protocol_definition * getProtocolDefinition(int protocol)
{
  for (int i = 0; ; i++) {
    if (multi_protocols[i].protocol == 0xFF)
      break;
    else if (multi_protocols[i].protocol == (unsigned int)protocol)
      return &multi_protocols[i];
  }

  return nullptr;
}

// static
QString Multiprotocols::getDefinitionVersion()
{
  return QString("%1.%2.%3.%4").arg(VERSION_MAJOR).arg(VERSION_MINOR).arg(VERSION_REVISION).arg(VERSION_PATCH_LEVEL);
}

// static
int Multiprotocols::getMaxChannelCount(int protocol, unsigned subType)
{
  if (subType == PROTO_DSM)
    return 12;
  else
    return 16;
}

// static
QString Multiprotocols::protocolToString(int protocol)
{
  const mm_protocol_definition * pd = getProtocolDefinition(protocol);

  for (int i = 0; pd->protocol == 0xFF; i++) {
    if (pd->protocol == protocol)
      return pd->ProtoString;
  }

  return CPN_STR_UNKNOWN_ITEM;
}

// static
QString Multiprotocols::subTypeToString(int protocol, unsigned subType)
{
  const mm_protocol_definition * pd = getProtocolDefinition(protocol);

  if (subType < pd->nbrSubProto) {
    const unsigned int sub_len = pd->SubProtoString[0];
    if (sub_len > 0) {
      return &pd->SubProtoString[subType * sub_len];
    }
  }

  return CPN_STR_UNKNOWN_ITEM;
}

/*
			OPTION_NONE		0	Hidden field
			OPTION_OPTION	1	"Option:"		value=-128..0(default)..127
			OPTION_RFTUNE	2	"RF freq tune:"	value=-128..0(default)..127
			OPTION_VIDFREQ	3	"Video freq:"	value=-128..0(default)..127
			OPTION_FIXEDID	4	"ID type:"		value="Auto":0(default), "Fixed":1
			OPTION_TELEM	5	"Telem:"		value="Off":0(default), "On":1, "Off+Aux":2, "On+Aux":3
			OPTION_SRVFREQ	6	"Servo freq(Hz):"	value="50":0(default).."400":70 => display=50+5*option with option=0..70
			OPTION_MAXTHR	7	"Max throw:"	value="Disabled":0, "Enabled":1
			OPTION_RFCHAN	8	"Select RF chan:"	value=-128..0(default)..127
			OPTION_RFPOWER	9	"RF power:"		"1.6mW":0(default),"2.0mW":1,"2.5mW":2,"3.2mW":3,"4.0mW":4,"5.0mW":5,"6.3mW":6,"7.9mW":7,"10mW\0":8,"13mW\0":9,"16mW\0":10,"20mW\0":11,"25mW\0":12,"32mW\0":13,"40mW\0":14,"50mW\0":15
			OPTION_WBUS		10	"Output:"		"WBUS":0(default),"PPM":1
*/

// static
QString Multiprotocols::optionTypeToString(int protocol, unsigned subType)
{
  const mm_protocol_definition * pd = getProtocolDefinition(protocol);

  switch (pd->optionType) {
    case MM_OPTION_NONE:
      return tr("NONE");
		case MM_OPTION_OPTION:
      return tr("Option");
		case MM_OPTION_RFTUNE:
      return tr("RF freq tune");
    case MM_OPTION_VIDFREQ:
      return tr("Video freq");
    case MM_OPTION_FIXEDID:
      return tr("ID type");
    case MM_OPTION_TELEM:
      return tr("Telemetry");
    case MM_OPTION_SRVFREQ:
      return tr("Servo freq(Hz)");
    case MM_OPTION_MAXTHR:
      return tr("Max throw");
		case MM_OPTION_RFCHAN:
      return tr("RF channel");
		case MM_OPTION_RFPOWER:
      return tr("RF power");
    case MM_OPTION_WBUS:
      return tr("Output");
    default:
      return CPN_STR_UNKNOWN_ITEM;
  }
}

int Multiprotocols::optionTypeMin(int protocol, unsigned subType)
{
  const mm_protocol_definition * pd = getProtocolDefinition(protocol);

  switch (pd->optionType) {
    case MM_OPTION_NONE:
    case MM_OPTION_FIXEDID:
    case MM_OPTION_MAXTHR:
    case MM_OPTION_RFPOWER:
    case MM_OPTION_SRVFREQ:
    case MM_OPTION_TELEM:
    case MM_OPTION_WBUS:
      return 0;
    default:
      return -128;
  }
}

int Multiprotocols::optionTypeMax(int protocol, unsigned subType)
{
  const mm_protocol_definition * pd = getProtocolDefinition(protocol);

  switch (pd->optionType) {
    case MM_OPTION_NONE:
      return 0;
    case MM_OPTION_FIXEDID:
    case MM_OPTION_TELEM:
    case MM_OPTION_WBUS:
      return 1;
    case MM_OPTION_MAXTHR:
      return 3;
    case MM_OPTION_RFPOWER:
      return 15;
    case MM_OPTION_SRVFREQ:
      return 70;
    default:
      return 127;
  }
}

FieldRange Multiprotocols::optionTypeRange(int protocol, unsigned subType)
{
  FieldRange result;

  result.min = optionTypeMin(protocol, subType);
  result.max = optionTypeMax(protocol, subType);

  return result;
}

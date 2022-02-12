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

#include "multiprotocols.h"
#include "compounditemmodels.h"

const int getProtocolCount()
{
  bool guardfound = false;
  int i = 0;

  for ( ; i <= 0xFF; i++) {
    if (multi_protocols[i].protocol == 0xFF && multi_protocols[i].ProtoString == nullptr && multi_protocols[i].SubProtoString == nullptr) {
      guardfound = true;
      break;
    }
  }

  if (guardfound)
    return i;
  else {
    qDebug() << "Error: guard entry 0xFF not found in multi_protocols array";
    return -1;
  }
}

const mm_protocol_definition * getProtocolDefinition(int protocol)
{
  const int cnt = getProtocolCount();

  for (int i = 0; i < cnt; i++) {
    if (multi_protocols[i].protocol == (unsigned int)protocol - 1)
      return &multi_protocols[i];
  }

  return nullptr;
}

// static
const mm_protocol_definition * Multiprotocols::getDefinition(int protocol)
{
  const int cnt = getProtocolCount();

  for (int i = 0; i < cnt; i++) {
    if (multi_protocols[i].protocol == (unsigned int)protocol - 1)
      return &multi_protocols[i];
  }

  qDebug() << "Multiprotocol" << protocol << "not found so using default";
  return &multi_protocols[PROTO_CONFIG];
}

// static
int Multiprotocols::getMaxChannelCount(int protocol, unsigned subType)
{
  if ((protocol - 1) == PROTO_DSM)
    return 12;

  return 16;
}

// static
QString Multiprotocols::protocolToString(int protocol)
{
  const mm_protocol_definition * pd = getProtocolDefinition(protocol);

  if (pd)
    return pd->ProtoString;

  return CPN_STR_UNKNOWN_ITEM;
}

//  static
AbstractStaticItemModel * Multiprotocols::protocolItemModel()
{
  AbstractStaticItemModel * mdl = new AbstractStaticItemModel();
  mdl->setName(AIM_MULTI_PROTOCOL);

  const int cnt = getProtocolCount();

  for (int i = 0; i < cnt; i++) {
    mdl->appendToItemList(multi_protocols[i].ProtoString, i);
  }

  mdl->sort(0);
  mdl->loadItemList();
  AbstractStaticItemModel::dumpItemModelContents(mdl);
  return mdl;
}

// static
int Multiprotocols::getNumSubTypes(int protocol)
{
  const mm_protocol_definition * pd = getProtocolDefinition(protocol);

  if (pd)
    return pd->nbrSubProto;
  else
    return 0;
}

// static
QString Multiprotocols::subTypeToString(int protocol, unsigned subType)
{
  const mm_protocol_definition * pd = getProtocolDefinition(protocol);

  if (pd) {
    if (subType < pd->nbrSubProto) {
      const unsigned int sub_len = pd->SubProtoString[0];
      if (sub_len > 0) {
        return &pd->SubProtoString[subType * sub_len];
      }
    }
  }

  return CPN_STR_UNKNOWN_ITEM;
}

//  static
AbstractStaticItemModel * Multiprotocols::subTypeItemModel()
{
  AbstractStaticItemModel * mdl = new AbstractStaticItemModel();
  mdl->setName(AIM_MULTI_SUBTYPE);

  const int cnt = getProtocolCount();

  for (int i = 0; i < cnt; i++) {
    for (unsigned int j = 0; j <= multi_protocols[i].nbrSubProto; j++) {
      //  load all protocols and subtypes then filter when used
      mdl->appendToItemList(subTypeToString(multi_protocols[i].protocol, j), j, true, 0, i);
    }
  }

  mdl->sort(0);
  mdl->loadItemList();
  AbstractStaticItemModel::dumpItemModelContents(mdl);
  return mdl;
}

// static
QString Multiprotocols::optionTypeToString(int protocol, unsigned subType)
{
  const mm_protocol_definition * pd = getProtocolDefinition(protocol);

  if (pd) {
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

  return CPN_STR_UNKNOWN_ITEM;
}

//  static
int Multiprotocols::optionTypeMin(int protocol, unsigned subType)
{
  const mm_protocol_definition * pd = getProtocolDefinition(protocol);

  if (pd) {
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

  return 0;
}

//  static
int Multiprotocols::optionTypeMax(int protocol, unsigned subType)
{
  const mm_protocol_definition * pd = getProtocolDefinition(protocol);

  if (pd) {
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

  return 0;
}

//  static
FieldRange Multiprotocols::optionTypeRange(int protocol, unsigned subType)
{
  FieldRange result;

  result.min = optionTypeMin(protocol, subType);
  result.max = optionTypeMax(protocol, subType);

  return result;
}

//  static
int Multiprotocols::optionTypeValueUiWidget(int optionType)
{
  switch (optionType) {
    case MM_OPTION_NONE:
      return VALUE_UI_WIDGET_NONE;
    case MM_OPTION_OPTION:
    case MM_OPTION_RFTUNE:
    case MM_OPTION_VIDFREQ:
    case MM_OPTION_RFCHAN:
      return VALUE_UI_WIDGET_SPINBOX;
    case MM_OPTION_FIXEDID:
    case MM_OPTION_TELEM:
    case MM_OPTION_SRVFREQ:
    case MM_OPTION_MAXTHR:
    case MM_OPTION_RFPOWER:
    case MM_OPTION_WBUS:
      return VALUE_UI_WIDGET_COMBOBOX;
    default:
      return VALUE_UI_WIDGET_NONE;
  }
}

//  static
QString Multiprotocols::optionValueToString(int optionType, int optionValue)
{
  QStringList strl;
  switch (optionType) {
    case MM_OPTION_FIXEDID:
      strl << tr("Auto") << tr("Fixed");
      break;
    case MM_OPTION_TELEM:
      strl << tr("Off") << tr("On") << tr("Off+Aux") << tr("On+Aux");
      break;
    case MM_OPTION_MAXTHR:
      strl << tr("Disabled") << tr("Enabled");
      break;
    case MM_OPTION_RFPOWER:
      strl << tr("1.6mW") << tr("2.0mW") << tr("2.5mW") << tr("3.2mW") << tr("4.0mW") << tr("5.0mW") << tr("6.3mW") << tr("7.9mW")
          << tr("10mW") << tr("13mW") << tr("16mW") << tr("20mW") << tr("25mW") << tr("32mW") << tr("40mW") << tr("50mW");
      break;
    case MM_OPTION_WBUS:
      strl << tr("WBUS") << tr("PPM");
      break;
  }

  if (optionValue >= 0 && optionValue < strl.count())
    return strl[optionValue];

  return CPN_STR_UNKNOWN_ITEM;
}

//  static
AbstractStaticItemModel * Multiprotocols::optionValueItemModel()
{
  AbstractStaticItemModel * mdl = new AbstractStaticItemModel();
  mdl->setName(AIM_MULTI_OPTIONTYPEVALUE);

  //  load all option types then filter when used
  for (int i = 0; i < MM_OPTION_COUNT; i++) {
    for (int j = 0; ; j++) {
      const QString str = optionValueToString(i, j);
      if (str != CPN_STR_UNKNOWN_ITEM)
        mdl->appendToItemList(optionValueToString(i, j), j, true, 0, i);
      else
        break;
    }
  }

  mdl->sort(0);
  mdl->loadItemList();
  AbstractStaticItemModel::dumpItemModelContents(mdl);
  return mdl;
}

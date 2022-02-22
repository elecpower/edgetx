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

#include "trainermoduledata.h"
#include "compounditemmodels.h"
#include "eeprominterface.h"
#include "generalsettings.h"

FieldRange TrainerModuleData::getChannelStartRange() const
{
  FieldRange result;

  result.min = 1;
  result.max = 29;
  result.prefix = "CH";

  return result;
}

FieldRange TrainerModuleData::getChannelsCountRange() const
{
  FieldRange result;

  result.min = 1;
  result.max = 8;

  return result;
}

FieldRange TrainerModuleData::getFrameLengthRange() const
{
  FieldRange result;

  result.min = 100;
  result.max = 800;
  result.step = 50;

  return result;
}

FieldRange TrainerModuleData::getDelayRange() const
{
  FieldRange result;

  result.min = 11.5;
  result.max = 40;
  result.step = 0.5;
  result.unit = "us";

  return result;
}

QString TrainerModuleData::modeToString() const
{
  return modeToString(mode);
}

//  static
QString TrainerModuleData::modeToString(int value)
{
  switch (value) {
    case TRAINERMODE_OFF:
      return tr("OFF");
    case TRAINERMODE_MASTER_JACK:
      return tr("Master/Jack");
    case TRAINERMODE_SLAVE_JACK:
      return tr("Slave/Jack");
    case TRAINERMODE_MASTER_SBUS_EXTERNAL_MODULE:
      return tr("Master/SBUS Module");
    case TRAINERMODE_MASTER_CPPM_EXTERNAL_MODULE:
      return tr("Master/CPPM Module");
    case TRAINERMODE_MASTER_BATTERY_COMPARTMENT:
      return tr("Master/Battery");
    case TRAINERMODE_MASTER_BLUETOOTH:
      return tr("Master/Bluetooth");
    case TRAINERMODE_SLAVE_BLUETOOTH:
      return tr("Slave/Bluetooth");
    case TRAINERMODE_MULTI:
      return tr("Master/Multi");
    default:
      return CPN_STR_UNKNOWN_ITEM;
  }
}

//  static
bool TrainerModuleData::isModeAvailable(const GeneralSettings & generalSettings, const Firmware * firmware, const int value)
{
  if (value < TRAINERMODE_FIRST || value > TRAINERMODE_LAST)
    return false;

  bool ret = true;
  const Board::Type board = firmware->getBoard();

  if (!IS_FLYSKY_NV14(board) && value == TRAINERMODE_OFF)
    ret = false;
  else if (!IS_TARANIS(board) || IS_ACCESS_RADIO(board, Firmware::getCurrentVariant()->getId())) {
    if (value >= TRAINERMODE_MASTER_SBUS_EXTERNAL_MODULE && value <= TRAINERMODE_MASTER_BATTERY_COMPARTMENT)
      ret = false;
  }
  else if (generalSettings.auxSerialMode != UART_MODE_SBUS_TRAINER && value == TRAINERMODE_MASTER_BATTERY_COMPARTMENT)
      ret = false;

  if (generalSettings.bluetoothMode != GeneralSettings::BLUETOOTH_MODE_TRAINER && value >= TRAINERMODE_MASTER_BLUETOOTH && value <= TRAINERMODE_SLAVE_BLUETOOTH)
      ret = false;

  if (!IS_RADIOMASTER_TX16S(board) && value == TRAINERMODE_MULTI)
      ret = false;

  return ret;
}

//  static
AbstractStaticItemModel * TrainerModuleData::modeItemModel(const GeneralSettings & generalSettings, const Firmware * firmware)
{
  AbstractStaticItemModel * mdl = new AbstractStaticItemModel();
  mdl->setName(AIM_MODELDATA_TRAINERMODE);

  for (int i = TRAINERMODE_FIRST; i <= TRAINERMODE_LAST; i++) {
    mdl->appendToItemList(modeToString(i), i, isModeAvailable(generalSettings, firmware, i));
  }

  mdl->loadItemList();
  return mdl;
}

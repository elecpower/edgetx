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

#include "datahelpers.h"

#include <QtCore>

class GeneralSettings;
class Firmware;
class AbstractStaticItemModel;

constexpr char AIM_MODELDATA_TRAINERMODE[]  {"trainermoduledata.mode"};

class TrainerModuleData {

  Q_DECLARE_TR_FUNCTIONS(TrainerModuleData)

  public:
    enum TrainerMode {
      TRAINERMODE_OFF,
      TRAINERMODE_FIRST = TRAINERMODE_OFF,
      TRAINERMODE_MASTER_JACK,
      TRAINERMODE_SLAVE_JACK,
      TRAINERMODE_MASTER_SBUS_EXTERNAL_MODULE,
      TRAINERMODE_MASTER_CPPM_EXTERNAL_MODULE,
      TRAINERMODE_MASTER_BATTERY_COMPARTMENT,
      TRAINERMODE_MASTER_BLUETOOTH,
      TRAINERMODE_SLAVE_BLUETOOTH,
      TRAINERMODE_MULTI,
      TRAINERMODE_LAST = TRAINERMODE_MULTI
    };

    unsigned int mode;
    unsigned int channelsStart;
    int channelsCount;
    int frameLength;
    int delay;
    unsigned int pulsePol;

    void clear() { memset(reinterpret_cast<void *>(this), 0, sizeof(TrainerModuleData)); }

    FieldRange getChannelStartRange() const;
    FieldRange getChannelsCountRange() const;
    FieldRange getFrameLengthRange() const;
    FieldRange getDelayRange() const;

    QString modeToString() const;
    static QString modeToString(const int value);
    static bool isModeAvailable(const GeneralSettings & generalSettings, const Firmware * firmware, const int value);
    static AbstractStaticItemModel * modeItemModel(const GeneralSettings & generalSettings, const Firmware * firmware);
};

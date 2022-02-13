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

#include <QString>

class GeneralSettings;
class AbstractStaticItemModel;

constexpr char AIM_MODELDATA_TRAINERMODE[]  {"trainermoduledata.mode"};

class TrainerModuleData {
  public:
    enum TrainerMode {
      TRAINER_MODE_OFF,
      TRAINER_MODE_FIRST = TRAINER_MODE_OFF,
      TRAINER_MODE_MASTER_JACK,
      TRAINER_MODE_SLAVE_JACK,
      TRAINER_MODE_MASTER_SBUS_EXTERNAL_MODULE,
      TRAINER_MODE_MASTER_CPPM_EXTERNAL_MODULE,
      TRAINER_MODE_MASTER_BATTERY_COMPARTMENT,
      TRAINER_MODE_MASTER_BLUETOOTH,
      TRAINER_MODE_SLAVE_BLUETOOTH,
      TRAINER_MODE_MULTI,
      TRAINER_MODE_LAST = TRAINER_MODE_MULTI
    };

    int mode;
    unsigned int channelsStart;
    int channelsCount;
    int frameLength;
    int delay;
    int pulsePol;

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

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

#include "modeledit.h"
#include "eeprominterface.h"
#include "compounditemmodels.h"
#include "filtereditemmodels.h"

namespace Ui {
  class FunctionSwitches;
}

class FunctionSwitchesPanel : public ModelPanel
{
    Q_OBJECT

  public:
    FunctionSwitchesPanel(QWidget * parent, ModelData & model, GeneralSettings & generalSettings, Firmware * firmware);
    virtual ~FunctionSwitchesPanel();

    virtual void update();
    void update(int index);

  signals:
    void updateDataModels();

  private slots:
    void on_nameEditingFinished();
    void on_configCurrentIndexChanged(int index);
    void on_startPosnCurrentIndexChanged(int index);
    void on_groupChanged(int value);
    void on_alwaysOnGroupChanged(int value);

  private:
    Ui::FunctionSwitches * ui;
    QVector<AutoLineEdit *> aleNames;
    QVector<QComboBox *> cboConfigs;
    QVector<QComboBox *> cboStartupPosns;
    QVector<QSpinBox *> sbGroups;
    QVector<QCheckBox *> cbAlwaysOnGroups;
    int switchcnt;
};



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

#include "setup.h"
#include "functionswitches.h"
#include "general.h"
#include "modules.h"
#include "timers.h"
#include "trainer.h"

SetupPanel::SetupPanel(QWidget * parent, ModelData & model, GeneralSettings & generalSettings, Firmware * firmware,
                       CompoundItemModelFactory * sharedItemModels) :
  ModelPanel(parent, model, generalSettings, firmware)
{

  QGridLayout * gridLayout = new QGridLayout(this);
  tabWidget = new QTabWidget(this);

  GeneralPanel * tab = new GeneralPanel(tabWidget, model, generalSettings, firmware, sharedItemModels);
  connect(tab, &GeneralPanel::modified, this, &SetupPanel::modified);
  tabWidget->addTab(tab);
  panels << tab;

  ModulesPanel * tab = new ModulesPanel(tabWidget, model, generalSettings, firmware, sharedItemModels);
  connect(tab, &ModulesPanel::modified, this, &SetupPanel::modified);
  tabWidget->addTab(tab);
  panels << tab;

  TimersPanel * tab = new TimersPanel(tabWidget, model, generalSettings, firmware, sharedItemModels);
  connect(tab, &TimersPanel::modified, this, &SetupPanel::modified);
  tabWidget->addTab(tab);
  panels << tab;

  TrainerPanel * tab = new TrainerPanel(tabWidget, model, generalSettings, firmware, sharedItemModels);
  connect(tab, &TrainerPanel::modified, this, &SetupPanel::modified);
  tabWidget->addTab(tab);
  panels << tab;

  FunctionSwitchesPanel * tab = new FunctionSwitchesPanel(tabWidget, model, generalSettings, firmware, sharedItemModels);
  connect(tab, &FunctionSwitchesPanel::modified, this, &SetupPanel::modified);
  tabWidget->addTab(tab);
  panels << tab;

  gridLayout->addWidget(tabWidget, 0, 0, 1, 1);
}

SetupPanel::~SetupPanel()
{

}

void SetupPanel::update()
{
  emit updated();
}

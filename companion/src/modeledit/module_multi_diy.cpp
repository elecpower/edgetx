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
#include "module_multi_diy.h"
#include "ui_module_multi_diy.h"

ModuleMultiDIY::ModuleMultiDIY(int protocol, unsigned int subType, int & optionTypeValue, QWidget * parent = nullptr);
  QWidget(parent),
  protocol(protocol),
  subType(subType),
  optionTypeValue(optionTypeValue),
  ui(new Ui::ModuleMultiDIY)
{
  ui->setupUi(this);

  int valueUiWidget = Multiprotocols::optionTypeValueUiWidget(int protocol, unsigned subType);

  if ( valueUiWidget != VALUE_UI_WIDGET_NONE) {

    if (optionTypeLabel) {
      optionTypeLabel->setVisible(true);
      optionTypeLabel->setText(Multiprotocols::optionTypeToString(int protocol, unsigned subType));
    }

    if (optionTypeValueSpin && valueUiWidget == VALUE_UI_WIDGET_SPINBOX) {
      FieldRange rng = Multiprotocols::optionTypeRange(int protocol, unsigned subType);
      optionTypeValueSpin->setMinimum(rng.min);
      optionTypeValueSpin->setMaximum(rng.max);
      connect(optionTypeValueSpin, &QSpinBox::editingFinished, this, [=]() {
        if (!lock) {
          optionTypeValue = optionTypeValueSpin->value();
          update();
        }
      });
    }
    else
      optionTypeValueSpin->setVisible(false);

    if (optionTypeValueCombo && valueUiWidget == VALUE_UI_WIDGET_COMBOBOX) {
      optionTypeValueCombo->setSizeAdjustPolicy(QComboBox::AdjustToContents);
      optionTypeValueCombo->setMaxVisibleItems(10);
      optionTypeValueCombo->setModel(Multiprotocols::optionTypeValueItemModel(int protocol, unsigned subType));
      connect(optionTypeValueCombo, &QComboBox::currentIndexChanged, this, this, [=]() {
        if (!lock) {
          optionTypeValue = optionTypeValueCombo->itemData(optionTypeValueCombo->currentIndex()).toInt();
          update();
        }
      });
    }
    else
      optionTypeValueCombo->setVisible(false);
  }
  else {
    optionTypeLabel->setVisible(false);
    optionTypeValueSpin->setVisible(false);
    optionTypeValueCombo->setVisible(false);
  }
}

ModuleMultiDIY::~ModuleMultiDIY()
{
  delete ui;
}

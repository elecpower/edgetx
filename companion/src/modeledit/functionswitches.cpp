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

#include "setup_functionswitches.h"
#include "ui_setup_functionswitches.h"


FunctionSwitchesPanel::FunctionSwitchesPanel(QWidget * parent, ModelData & model, GeneralSettings & generalSettings, Firmware * firmware):
  ModelPanel(parent, model, generalSettings, firmware),
  ui(new Ui::FunctionSwitches)
{
  ui->setupUi(this);

  AbstractStaticItemModel *fsConfig = ModelData::funcSwitchConfigItemModel();
  AbstractStaticItemModel *fsStart = ModelData::funcSwitchStartItemModel();

  lock = true;

  QRegExp rx(CHAR_FOR_NAMES_REGEX);

  switchcnt = Boards::getCapability(firmware->getBoard(), Board::FunctionSwitches);

  for (int i = 0; i < switchcnt; i++) {
    QLabel * lblSwitchId = new QLabel(this);
    lblSwitchId->setText(tr("SW%1").arg(i + 1));

    AutoLineEdit * aleName = new AutoLineEdit(this);
    aleName->setProperty("index", i);
    aleName->setValidator(new QRegExpValidator(rx, this));
    aleName->setField((char *)model.functionSwitchNames[i], 3);

    QComboBox * cboConfig = new QComboBox(this);
    cboConfig->setProperty("index", i);
    cboConfig->setModel(fsConfig);

    QComboBox * cboStartPosn = new QComboBox(this);
    cboStartPosn->setProperty("index", i);
    cboStartPosn->setModel(fsStart);

    QSpinBox * sbGroup = new QSpinBox(this);
    sbGroup->setProperty("index", i);
    sbGroup->setMaximum(3);
    sbGroup->setSpecialValueText("-");

    QCheckBox * cbAlwaysOnGroup = new QCheckBox(this);
    cbAlwaysOnGroup->setProperty("index", i);

    int row = 0;
    int coloffset = 1;
    ui->gridSwitches->addWidget(lblSwitchId, row++, i + coloffset);
    ui->gridSwitches->addWidget(aleName, row++, i + coloffset);
    ui->gridSwitches->addWidget(cboConfig, row++, i + coloffset);
    ui->gridSwitches->addWidget(cboStartPosn, row++, i + coloffset);
    ui->gridSwitches->addWidget(sbGroup, row++, i + coloffset);
    ui->gridSwitches->addWidget(cbAlwaysOnGroup, row++, i + coloffset);

    connect(aleName, &AutoLineEdit::currentDataChanged, this, &FunctionSwitchesPanel::on_nameEditingFinished);
    connect(cboConfig, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &FunctionSwitchesPanel::on_configCurrentIndexChanged);
    connect(cboStartPosn, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &FunctionSwitchesPanel::on_startPosnCurrentIndexChanged);
    connect(sbGroup, QOverload<int>::of(&QSpinBox::valueChanged), this, &FunctionSwitchesPanel::on_groupChanged);
    connect(cbAlwaysOnGroup, &QCheckBox::toggled, this, &FunctionSwitchesPanel::on_alwaysOnGroupChanged);

    aleNames << aleName;
    cboConfigs << cboConfig;
    cboStartupPosns << cboStartPosn;
    sbGroups << sbGroup;
    cbAlwaysOnGroups << cbAlwaysOnGroup;
  }

  update();

  lock = false;
}

FunctionSwitchesPanel::~FunctionSwitchesPanel()
{
  delete ui;
}

void FunctionSwitchesPanel::update()
{
  for (int i = 0; i < switchcnt; i++) {
    update(i);
  }
}

void FunctionSwitchesPanel::update(int index)
{
  lock = true;

  for (int i = 0; i < switchcnt; i++) {
    aleNames[i]->update();
    cboConfigs[i]->setCurrentIndex(model->getFuncSwitchConfig(i));
    cboStartupPosns[i]->setCurrentIndex(model->getFuncSwitchStart(i));
    unsigned int grp = model->getFuncSwitchGroup(i);
    sbGroups[i]->setValue(grp);
    cbAlwaysOnGroups[i]->setChecked(model->getFuncSwitchAlwaysOnGroup(i));

    if (cboConfigs[i]->currentIndex() < 2)
      cboStartupPosns[i]->setEnabled(false);
    else
      cboStartupPosns[i]->setEnabled(true);

    if (cboConfigs[i]->currentIndex() < 1)
      sbGroups[i]->setEnabled(false);
    else
      sbGroups[i]->setEnabled(true);

    if (!(sbGroups[i]->isEnabled()) || grp < 1)
      cbAlwaysOnGroups[i]->setEnabled(false);
    else
      cbAlwaysOnGroups[i]->setEnabled(true);
  }

  lock = false;
}

void FunctionSwitchesPanel::on_nameEditingFinished()
 {
   emit updateDataModels();
 }


void FunctionSwitchesPanel::on_configCurrentIndexChanged(int index)
{
  if (!sender())
    return;

  QComboBox * cb = qobject_cast<QComboBox *>(sender());

  if (cb && !lock) {
    lock = true;
    bool ok = false;
    unsigned int i = sender()->property("index").toInt(&ok);
      if (ok && model->getFuncSwitchConfig(i) != (unsigned int)index) {
        model->setFuncSwitchConfig(i, index);
      if (index < 2)
          model->setFuncSwitchStart(i, ModelData::FUNC_SWITCH_START_INACTIVE);
      if (index < 1)
          model->setFuncSwitchGroup(i, 0);
      update(i);
      emit modified();
      emit updateDataModels();
     }
    lock = false;
  }
}

void FunctionSwitchesPanel::on_startPosnCurrentIndexChanged(int index)
{
  if (!sender())
    return;

  QComboBox * cb = qobject_cast<QComboBox *>(sender());

  if (cb && !lock) {
    lock = true;
    bool ok = false;
    unsigned int i = sender()->property("index").toInt(&ok);
    if (ok && model->getFuncSwitchStart(i) != (unsigned int)index) {
      model->setFuncSwitchStart(i, index);
      emit modified();
    }
    lock = false;
  }
}

void FunctionSwitchesPanel::on_groupChanged(int value)
{
  if (!sender())
    return;

  QSpinBox * sb = qobject_cast<QSpinBox *>(sender());

  if (sb && !lock) {
    lock = true;
    bool ok = false;
    int i = sender()->property("index").toInt(&ok);
    if (ok && model->getFuncSwitchGroup(i) != (unsigned int)value) {
      model->setFuncSwitchGroup(i, (unsigned int)value);
      update(i);
      emit modified();
    }
    lock = false;
  }
}

void FunctionSwitchesPanel::on_alwaysOnGroupChanged(int value)
{
  if (!sender())
    return;

  QCheckBox * cb = qobject_cast<QCheckBox *>(sender());

  if (cb && !lock) {
    lock = true;
    bool ok = false;
    int i = sender()->property("index").toInt(&ok);

    if (ok) {
      model->setFuncSwitchAlwaysOnGroup(i, (unsigned int)value);
      update();
      emit modified();
    }

    lock = false;
  }
}

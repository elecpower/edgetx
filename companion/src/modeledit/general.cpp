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
#include "ui_setup.h"
#include "setup_timers.h"
#include "setup_modules.h"
#include "setup_functionswitches.h"
#include "setup_trainer.h"
#include "appdata.h"
#include "modelprinter.h"
#include "checklistdialog.h"
#include "helpers.h"

#include <QDir>

constexpr char FIM_THRSOURCE[]   {"Throttle Source"};

GeneralPanel::GeneralPanel(QWidget * parent, ModelData & model, GeneralSettings & generalSettings, Firmware * firmware,
                       CompoundItemModelFactory * sharedItemModels) :
  ModelPanel(parent, model, generalSettings, firmware),
  ui(new Ui::Setup),
  sharedItemModels(sharedItemModels)
{
  ui->setupUi(this);

  lock = true;

  panelFilteredModels = new FilteredItemModelFactory();

  panelFilteredModels->registerItemModel(new FilteredItemModel(sharedItemModels->getItemModel(AbstractItemModel::IMID_RawSwitch),
                                                               RawSwitch::AllSwitchContexts),
                                         FIM_TIMERSWITCH);
  connectItemModelEvents(panelFilteredModels->getItemModel(FIM_TIMERSWITCH));

  panelFilteredModels->registerItemModel(new FilteredItemModel(sharedItemModels->getItemModel(AbstractItemModel::IMID_ThrSource)),
                                         FIM_THRSOURCE);
  connectItemModelEvents(panelFilteredModels->getItemModel(FIM_THRSOURCE));

  Board::Type board = firmware->getBoard();

  memset(modules, 0, sizeof(modules));

  QRegExp rx(CHAR_FOR_NAMES_REGEX);
  ui->name->setValidator(new QRegExpValidator(rx, this));
  ui->name->setMaxLength(firmware->getCapability(ModelName));

  if (firmware->getCapability(HasModelImage)) {
    if (Boards::getCapability(board, Board::HasColorLcd)) {
      ui->imagePreview->setFixedSize(QSize(192, 114));
    }
    else {
      ui->imagePreview->setFixedSize(QSize(64, 32));
    }
    QStringList items;
    items.append("");
    QString path = g.profile[g.id()].sdPath();
    path.append("/IMAGES/");
    QDir qd(path);
    if (qd.exists()) {
      QStringList filters = firmware->getCapabilityStr(ModelImageFilters).split("|");
      foreach ( QString file, qd.entryList(filters, QDir::Files) ) {
        QFileInfo fi(file);
        QString temp;
        if (firmware->getCapability(ModelImageKeepExtn))
          temp = fi.fileName();
        else
          temp = fi.completeBaseName();
        if (!items.contains(temp) && temp.length() <= firmware->getCapability(ModelImageNameLen))
          items.append(temp);
      }
    }
    if (!items.contains(model.bitmap)) {
      items.append(model.bitmap);
    }
    items.sort(Qt::CaseInsensitive);
    foreach (QString file, items) {
      ui->image->addItem(file);
      if (file == model.bitmap) {
        ui->image->setCurrentIndex(ui->image->count() - 1);
        if (!file.isEmpty()) {
          QString fileName = path;
          fileName.append(model.bitmap);
          if (!firmware->getCapability(ModelImageKeepExtn)) {
            QString extn = firmware->getCapabilityStr(ModelImageFilters);
            if (extn.size() > 0)
              extn.remove(0, 1);  //  remove *
            fileName.append(extn);
          }
          QImage image(fileName);
          if (!image.isNull()) {
            ui->imagePreview->setPixmap(QPixmap::fromImage(image.scaled(ui->imagePreview->size())));
          }
        }
      }
    }
  }
  else {
    ui->image->hide();
    ui->modelImage_label->hide();
    ui->imagePreview->hide();
  }

  QWidget * prevFocus = ui->image;

   if (firmware->getCapability(HasTopLcd)) {
    ui->toplcdTimer->setField(model.toplcdTimer, this);
    for (int i = 0; i < CPN_MAX_TIMERS; i++) {
      if (i < timersCount) {
        ui->toplcdTimer->addItem(tr("Timer %1").arg(i + 1), i);
      }
    }
  }
  else {
    ui->toplcdTimerLabel->hide();
    ui->toplcdTimer->hide();
  }

  ui->throttleSource->setModel(panelFilteredModels->getItemModel(FIM_THRSOURCE));
  ui->throttleSource->setField(model.thrTraceSrc, this);

  if (!firmware->getCapability(HasDisplayText)) {
    ui->displayText->hide();
    ui->editText->hide();
  }

  if (!firmware->getCapability(GlobalFunctions)) {
    ui->gfEnabled->hide();
  }

  if (!firmware->getCapability(HasADCJitterFilter))
  {
    ui->jitterFilter->hide();
  }

  // Beep Center checkboxes
  prevFocus = ui->trimsDisplay;
  int analogs = CPN_MAX_STICKS + getBoardCapability(board, Board::Pots) + getBoardCapability(board, Board::Sliders);
  int genAryIdx = 0;
  for (int i = 0; i < analogs + firmware->getCapability(RotaryEncoders); i++) {
    RawSource src((i < analogs) ? SOURCE_TYPE_STICK : SOURCE_TYPE_ROTARY_ENCODER, (i < analogs) ? i : analogs - i);
    QCheckBox * checkbox = new QCheckBox(this);
    checkbox->setProperty("index", i);
    checkbox->setText(src.toString(&model, &generalSettings));
    ui->centerBeepLayout->addWidget(checkbox, 0, i + 1);
    connect(checkbox, SIGNAL(toggled(bool)), this, SLOT(onBeepCenterToggled(bool)));
    centerBeepCheckboxes << checkbox;
    if (IS_HORUS_OR_TARANIS(board)) {
      if (src.isPot(&genAryIdx) && !generalSettings.isPotAvailable(genAryIdx)) {
        checkbox->hide();
      }
      else if (src.isSlider(&genAryIdx) && !generalSettings.isSliderAvailable(genAryIdx)) {
        checkbox->hide();
      }
    }
    QWidget::setTabOrder(prevFocus, checkbox);
    prevFocus = checkbox;
  }

  // Startup switches warnings
  for (int i = 0; i < getBoardCapability(board, Board::Switches); i++) {
    Board::SwitchInfo switchInfo = Boards::getSwitchInfo(board, i);
    switchInfo.config = Board::SwitchType(generalSettings.switchConfig[i]);
    if (switchInfo.config == Board::SWITCH_NOT_AVAILABLE || switchInfo.config == Board::SWITCH_TOGGLE) {
      model.switchWarningEnable |= (1 << i);
      continue;
    }
    RawSource src(RawSourceType::SOURCE_TYPE_SWITCH, i);
    QLabel * label = new QLabel(this);
    QSlider * slider = new QSlider(this);
    QCheckBox * cb = new QCheckBox(this);
    slider->setProperty("index", i);
    slider->setOrientation(Qt::Vertical);
    slider->setMinimum(0);
    slider->setInvertedAppearance(true);
    slider->setInvertedControls(true);
    slider->setTickPosition(QSlider::TicksBothSides);
    slider->setMinimumSize(QSize(30, 50));
    slider->setMaximumSize(QSize(50, 50));
    slider->setSingleStep(1);
    slider->setPageStep(1);
    slider->setTickInterval(1);
    label->setText(src.toString(&model, &generalSettings));
    slider->setMaximum(switchInfo.config == Board::SWITCH_3POS ? 2 : 1);
    cb->setProperty("index", i);
    ui->switchesStartupLayout->addWidget(label, 0, i + 1);
    ui->switchesStartupLayout->setAlignment(label, Qt::AlignCenter);
    ui->switchesStartupLayout->addWidget(slider, 1, i + 1);
    ui->switchesStartupLayout->setAlignment(slider, Qt::AlignCenter);
    ui->switchesStartupLayout->addWidget(cb, 2, i + 1);
    ui->switchesStartupLayout->setAlignment(cb, Qt::AlignCenter);
    connect(slider, SIGNAL(valueChanged(int)), this, SLOT(startupSwitchEdited(int)));
    connect(cb, SIGNAL(toggled(bool)), this, SLOT(startupSwitchToggled(bool)));
    startupSwitchesSliders << slider;
    startupSwitchesCheckboxes << cb;
    QWidget::setTabOrder(prevFocus, slider);
    QWidget::setTabOrder(slider, cb);
    prevFocus = cb;
  }

  // Pot warnings
  prevFocus = ui->potWarningMode;
  if (IS_HORUS_OR_TARANIS(board)) {
    for (int i = 0; i < getBoardCapability(board, Board::Pots) + getBoardCapability(board, Board::Sliders); i++) {
      RawSource src(SOURCE_TYPE_STICK, CPN_MAX_STICKS + i);
      QCheckBox * cb = new QCheckBox(this);
      cb->setProperty("index", i);
      cb->setText(src.toString(&model, &generalSettings));
      ui->potWarningLayout->addWidget(cb, 0, i + 1);
      connect(cb, SIGNAL(toggled(bool)), this, SLOT(potWarningToggled(bool)));
      potWarningCheckboxes << cb;
      if (src.isPot(&genAryIdx) && !generalSettings.isPotAvailable(genAryIdx)) {
        cb->hide();
      }
      else if (src.isSlider(&genAryIdx) && !generalSettings.isSliderAvailable(genAryIdx)) {
        cb->hide();
      }
      QWidget::setTabOrder(prevFocus, cb);
      prevFocus = cb;
    }
  }
  else {
    ui->label_potWarning->hide();
    ui->potWarningMode->hide();
  }

  ui->trimsDisplay->setField(model.trimsDisplay, this);

  if (Boards::getCapability(firmware->getBoard(), Board::FunctionSwitches) > 0) {
    funcswitches = new FunctionSwitchesPanel(this, model, generalSettings, firmware);
    ui->functionSwitchesLayout->addWidget(funcswitches);
    connect(funcswitches, &FunctionSwitchesPanel::modified, this, &GeneralPanel::modified);
    connect(funcswitches, &FunctionSwitchesPanel::updateDataModels, this, &GeneralPanel::onFunctionSwitchesUpdateItemModels);
  }

  for (int i = firmware->getCapability(NumFirstUsableModule); i < firmware->getCapability(NumModules); i++) {
    modules[i] = new ModulePanel(this, model, model.moduleData[i], generalSettings, firmware, i, panelFilteredModels);
    ui->modulesLayout->addWidget(modules[i]);
    connect(modules[i], &ModulePanel::modified, this, &GeneralPanel::modified);
    connect(modules[i], &ModulePanel::updateItemModels, this, &GeneralPanel::onModuleUpdateItemModels);
    connect(this, &GeneralPanel::extendedLimitsToggled, modules[i], &ModulePanel::onExtendedLimitsToggled);
  }

  for (int i = firmware->getCapability(NumFirstUsableModule); i < firmware->getCapability(NumModules); i++) {
    for (int j = firmware->getCapability(NumFirstUsableModule); j < firmware->getCapability(NumModules); j++) {
      if (i != j) {
        connect(modules[i], SIGNAL(failsafeModified(unsigned)), modules[j], SLOT(onFailsafeModified(unsigned)));
      }
    }
  }

  if (firmware->getCapability(ModelTrainerEnable)) {
    modules[CPN_MAX_MODULES] = new ModulePanel(this, model, model.moduleData[CPN_MAX_MODULES], generalSettings, firmware, -1, panelFilteredModels);
    ui->modulesLayout->addWidget(modules[CPN_MAX_MODULES]);
    connect(modules[CPN_MAX_MODULES], &ModulePanel::modified, this, &GeneralPanel::modified);
  }

  disableMouseScrolling();

  lock = false;
}

GeneralPanel::~GeneralPanel()
{
  delete ui;
  delete panelFilteredModels;
  delete panelItemModels;
}

void GeneralPanel::on_extendedLimits_toggled(bool checked)
{
  model->extendedLimits = checked;
  emit extendedLimitsToggled();
  emit modified();
}

void GeneralPanel::on_throttleWarning_toggled(bool checked)
{
  model->disableThrottleWarning = !checked;
  emit modified();
}

void GeneralPanel::on_enableCustomThrottleWarning_toggled(bool checked)
{
  model->enableCustomThrottleWarning = checked;
  emit modified();
}

void GeneralPanel::on_customThrottleWarningPosition_valueChanged(int value)
{
  model->customThrottleWarningPosition = value;
  emit modified();
}

void GeneralPanel::on_throttleReverse_toggled(bool checked)
{
  model->throttleReversed = checked;
  emit modified();
}

void GeneralPanel::on_extendedTrims_toggled(bool checked)
{
  model->extendedTrims = checked;
  emit modified();
}

void GeneralPanel::on_trimIncrement_currentIndexChanged(int index)
{
  model->trimInc = index - 2;
  emit modified();
}

void GeneralPanel::on_throttleTrimSwitch_currentIndexChanged(int index)
{
  if (!lock) {
    model->thrTrimSwitch = ui->throttleTrimSwitch->currentData().toUInt();
    emit modified();
  }
}

void GeneralPanel::on_name_editingFinished()
{
  if (QString(model->name) != ui->name->text()) {
    int length = ui->name->maxLength();
    strncpy(model->name, ui->name->text().toLatin1(), length);
    emit modified();
  }
}

void GeneralPanel::on_image_currentIndexChanged(int index)
{
  if (!lock) {
    memset(model->bitmap, 0, CPN_MAX_BITMAP_LEN);
    strncpy(model->bitmap, ui->image->currentText().toLatin1(), CPN_MAX_BITMAP_LEN);
    if (model->bitmap[0] != '\0') {
      QString path = g.profile[g.id()].sdPath();
      path.append("/IMAGES/");
      path.append(model->bitmap);
      if (!firmware->getCapability(ModelImageKeepExtn)) {
        QString extn = firmware->getCapabilityStr(ModelImageFilters);
        if (extn.size() > 0)
          extn.remove(0, 1);  //  remove *
        path.append(extn);
      }
      QImage image(path);
      if (!image.isNull())
        ui->imagePreview->setPixmap(QPixmap::fromImage(image.scaled(ui->imagePreview->size())));
      else
        ui->imagePreview->clear();
    }
    else {
      ui->imagePreview->clear();
    }
    emit modified();
  }
}

void GeneralPanel::populateThrottleTrimSwitchCB()
{
  Board::Type board = firmware->getBoard();
  lock = true;
  ui->throttleTrimSwitch->clear();
  int idx=0;
  QString trim;
  for (int i=0; i<getBoardCapability(board, Board::NumTrims); i++, idx++) {
    // here order is TERA instead of RETA
    if (i == 0)
      trim = RawSource(SOURCE_TYPE_TRIM, 2).toString(model, &generalSettings);
    else if (i == 2)
      trim = RawSource(SOURCE_TYPE_TRIM, 0).toString(model, &generalSettings);
    else
      trim = RawSource(SOURCE_TYPE_TRIM, i).toString(model, &generalSettings);
    ui->throttleTrimSwitch->addItem(trim, idx);
  }

  int thrTrimSwitchIdx = ui->throttleTrimSwitch->findData(model->thrTrimSwitch);
  ui->throttleTrimSwitch->setCurrentIndex(thrTrimSwitchIdx);
  lock = false;
}

void GeneralPanel::update()
{
  ui->name->setText(model->name);
  ui->throttleReverse->setChecked(model->throttleReversed);
  ui->throttleSource->updateValue();
  populateThrottleTrimSwitchCB();
  ui->throttleWarning->setChecked(!model->disableThrottleWarning);
  ui->enableCustomThrottleWarning->setChecked(model->enableCustomThrottleWarning);
  ui->customThrottleWarningPosition->setValue(model->customThrottleWarningPosition);
  ui->trimIncrement->setCurrentIndex(model->trimInc+2);
  ui->throttleTrim->setChecked(model->thrTrim);
  ui->extendedLimits->setChecked(model->extendedLimits);
  ui->extendedTrims->setChecked(model->extendedTrims);
  ui->displayText->setChecked(model->displayChecklist);
  ui->gfEnabled->setChecked(!model->noGlobalFunctions);
  ui->jitterFilter->setCurrentIndex(model->jitterFilter);

  updateBeepCenter();
  updateStartupSwitches();

  if (IS_HORUS_OR_TARANIS(firmware->getBoard())) {
    updatePotWarnings();
  }

  for (int i = 0; i < CPN_MAX_MODULES + 1; i++) {
    if (modules[i]) {
      modules[i]->update();
    }
  }

  emit updated();
}

void GeneralPanel::updateBeepCenter()
{
  for (int i = 0; i < centerBeepCheckboxes.size(); i++) {
    centerBeepCheckboxes[i]->setChecked(model->beepANACenter & (0x01 << i));
  }
}

void GeneralPanel::updateStartupSwitches()
{
  lock = true;

  uint64_t switchStates = model->switchWarningStates;
  uint64_t value;

  for (int i = 0; i < startupSwitchesSliders.size(); i++) {
    QSlider * slider = startupSwitchesSliders[i];
    QCheckBox * cb = startupSwitchesCheckboxes[i];
    int index = slider->property("index").toInt();
    bool enabled = !(model->switchWarningEnable & (1 << index));
    if (IS_HORUS_OR_TARANIS(firmware->getBoard())) {
      value = (switchStates >> (2 * index)) & 0x03;
      if (generalSettings.switchConfig[index] != Board::SWITCH_3POS && value == 2) {
        value = 1;
      }
    }
    else {
      value = (i == 0 ? switchStates & 0x3 : switchStates & 0x1);
      switchStates >>= (i == 0 ? 2 : 1);
    }
    slider->setValue(value);
    slider->setEnabled(enabled);
    cb->setChecked(enabled);
  }

  lock = false;
}

void GeneralPanel::startupSwitchEdited(int value)
{
  if (!lock) {
    int shift = 0;
    uint64_t mask;
    int index = sender()->property("index").toInt();

    if (IS_HORUS_OR_TARANIS(firmware->getBoard())) {
      shift = index * 2;
      mask = 0x03ull << shift;
    }
    else {
      if (index == 0) {
        mask = 0x03;
      }
      else {
        shift = index + 1;
        mask = 0x01ull << shift;
      }
    }

    model->switchWarningStates &= ~mask;

    if (IS_HORUS_OR_TARANIS(firmware->getBoard()) && generalSettings.switchConfig[index] != Board::SWITCH_3POS) {
      if (value == 1) {
        value = 2;
      }
    }

    if (value) {
      model->switchWarningStates |= ((uint64_t)value << shift);
    }

    updateStartupSwitches();
    emit modified();
  }
}

void GeneralPanel::startupSwitchToggled(bool checked)
{
  if (!lock) {
    int index = sender()->property("index").toInt();

    if (checked)
      model->switchWarningEnable &= ~(1 << index);
    else
      model->switchWarningEnable |= (1 << index);

    updateStartupSwitches();
    emit modified();
  }
}

void GeneralPanel::updatePotWarnings()
{
  lock = true;
  ui->potWarningMode->setCurrentIndex(model->potsWarningMode);
  for (int i = 0; i < potWarningCheckboxes.size(); i++) {
    QCheckBox *checkbox = potWarningCheckboxes[i];
    int index = checkbox->property("index").toInt();
    checkbox->setChecked(!model->potsWarnEnabled[index]);
    checkbox->setDisabled(model->potsWarningMode == 0);
  }
  lock = false;
}

void GeneralPanel::potWarningToggled(bool checked)
{
  if (!lock) {
    int index = sender()->property("index").toInt();
    model->potsWarnEnabled[index] = !checked;
    updatePotWarnings();
    emit modified();
  }
}

void GeneralPanel::on_potWarningMode_currentIndexChanged(int index)
{
  if (!lock) {
    model->potsWarningMode = index;
    updatePotWarnings();
    emit modified();
  }
}

void GeneralPanel::on_displayText_toggled(bool checked)
{
  model->displayChecklist = checked;
  emit modified();
}

void GeneralPanel::on_gfEnabled_toggled(bool checked)
{
  model->noGlobalFunctions = !checked;
  emit modified();
}

void GeneralPanel::on_jitterFilter_currentIndexChanged(int index)
{
  if (!lock) {
    model->jitterFilter = ui->jitterFilter->currentIndex();
    emit modified();
  }
}

void GeneralPanel::on_throttleTrim_toggled(bool checked)
{
  model->thrTrim = checked;
  emit modified();
}

void GeneralPanel::onBeepCenterToggled(bool checked)
{
  if (!lock) {
    int index = sender()->property("index").toInt();
    unsigned int mask = (0x01 << index);
    if (checked)
      model->beepANACenter |= mask;
    else
      model->beepANACenter &= ~mask;
    emit modified();
  }
}

void GeneralPanel::on_editText_clicked()
{
  const QString path = Helpers::getChecklistsPath();
  QDir d(path);
  if (!d.exists()) {
    QMessageBox::critical(this, tr("Profile Settings"), tr("SD structure path not specified or invalid"));
  }
  else {
    ChecklistDialog *g = new ChecklistDialog(this, model);
    g->exec();
  }
}

void GeneralPanel::connectItemModelEvents(const FilteredItemModel * itemModel)
{
  connect(itemModel, &FilteredItemModel::aboutToBeUpdated, this, &GeneralPanel::onItemModelAboutToBeUpdated);
  connect(itemModel, &FilteredItemModel::updateComplete, this, &GeneralPanel::onItemModelUpdateComplete);
}

void GeneralPanel::onItemModelAboutToBeUpdated()
{
}

void GeneralPanel::onItemModelUpdateComplete()
{
}

void GeneralPanel::updateItemModels()
{
  sharedItemModels->update(AbstractItemModel::IMUE_Timers);
  emit updated();
}

void GeneralPanel::onModuleUpdateItemModels()
{
  sharedItemModels->update(AbstractItemModel::IMUE_Modules);
}

void GeneralPanel::onFunctionSwitchesUpdateItemModels()
{
  sharedItemModels->update(AbstractItemModel::IMUE_FunctionSwitches);
}

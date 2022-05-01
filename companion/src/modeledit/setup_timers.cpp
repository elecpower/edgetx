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

#include "setup_timers.h"
#include "ui_setup_timer.h"

constexpr char FIM_TIMERSWITCH[] {"Timer Switch"};

TimerPanel::TimerPanel(QWidget * parent, ModelData & model, TimerData & timer, GeneralSettings & generalSettings, Firmware * firmware,
                       QWidget * prevFocus, FilteredItemModelFactory * panelFilteredModels, CompoundItemModelFactory * panelItemModels):
  ModelPanel(parent, model, generalSettings, firmware),
  timer(timer),
  ui(new Ui::Timer),
  modelsUpdateCnt(0)
{
  ui->setupUi(this);
  connectItemModelEvents(panelFilteredModels->getItemModel(FIM_TIMERSWITCH));

  lock = true;

  // Name
  int length = firmware->getCapability(TimersName);
  if (length == 0)
    ui->name->hide();
  else {
    ui->name->setField(timer.name, length, this);
    connect(ui->name, SIGNAL(currentDataChanged()), this, SLOT(onNameChanged()));
  }

  ui->value->setField(timer.val, this);
  ui->value->setMaximumTime(firmware->getMaxTimerStart());

  ui->mode->setModel(panelItemModels->getItemModel(AIM_TIMER_MODE));
  ui->mode->setField(timer.mode, this);
  connect(ui->mode, SIGNAL(currentDataChanged(int)), this, SLOT(onModeChanged(int)));

  ui->swtch->setModel(panelFilteredModels->getItemModel(FIM_TIMERSWITCH));
  ui->swtch->setField(timer.swtch, this);

  ui->countdownBeep->setModel(panelItemModels->getItemModel(AIM_TIMER_COUNTDOWNBEEP));
  ui->countdownBeep->setField(timer.countdownBeep, this);
  connect(ui->countdownBeep, SIGNAL(currentDataChanged(int)), this, SLOT(onCountdownBeepChanged(int)));

  ui->minuteBeep->setField(timer.minuteBeep, this);

  if (firmware->getCapability(PermTimers)) {
    ui->persistent->setModel(panelItemModels->getItemModel(AIM_TIMER_PERSISTENT));
    ui->persistent->setField(timer.persistent, this);
  }
  else {
    ui->persistent->hide();
    ui->persistentValue->hide();
  }

  ui->countdownStart->setModel(panelItemModels->getItemModel(AIM_TIMER_COUNTDOWNSTART));
  ui->countdownStart->setField(timer.countdownStart, this);

  disableMouseScrolling();
  QWidget::setTabOrder(prevFocus, ui->name);
  QWidget::setTabOrder(ui->name, ui->value);
  QWidget::setTabOrder(ui->value, ui->mode);
  QWidget::setTabOrder(ui->mode, ui->countdownBeep);
  QWidget::setTabOrder(ui->countdownBeep, ui->countdownStart);
  QWidget::setTabOrder(ui->countdownStart, ui->minuteBeep);
  QWidget::setTabOrder(ui->minuteBeep, ui->persistent);

  update();
  lock = false;
}

TimerPanel::~TimerPanel()
{
  delete ui;
}

void TimerPanel::update()
{
  lock = true;

  ui->name->updateValue();
  ui->mode->updateValue();
  ui->swtch->updateValue();
  ui->value->updateValue();
  ui->countdownBeep->updateValue();
  ui->minuteBeep->updateValue();
  ui->countdownStart->updateValue();

  if (timer.mode != TimerData::TIMERMODE_OFF)
    ui->swtch->setEnabled(true);
  else
    ui->swtch->setEnabled(false);

  if (timer.countdownBeep == TimerData::COUNTDOWNBEEP_SILENT) {
    ui->countdownStartLabel->setEnabled(false);
    ui->countdownStart->setEnabled(false);
  }
  else {
    ui->countdownStartLabel->setEnabled(true);
    ui->countdownStart->setEnabled(true);
  }

  if (firmware->getCapability(PermTimers)) {
    ui->persistent->updateValue();
    ui->persistentValue->setText(timer.pvalueToString());
  }

  lock = false;
}

QWidget * TimerPanel::getLastFocus()
{
  return ui->persistent;
}

void TimerPanel::connectItemModelEvents(const FilteredItemModel * itemModel)
{
  connect(itemModel, &FilteredItemModel::aboutToBeUpdated, this, &TimerPanel::onItemModelAboutToBeUpdated);
  connect(itemModel, &FilteredItemModel::updateComplete, this, &TimerPanel::onItemModelUpdateComplete);
}

void TimerPanel::onItemModelAboutToBeUpdated()
{
  lock = true;
  modelsUpdateCnt++;
}

void TimerPanel::onItemModelUpdateComplete()
{
  modelsUpdateCnt--;
  if (modelsUpdateCnt < 1) {
    update();
    lock = false;
  }
}

void TimerPanel::onNameChanged()
{
  emit nameChanged();
}

void TimerPanel::onCountdownBeepChanged(int index)
{
  timer.countdownBeepChanged();
  update();
}

void TimerPanel::onModeChanged(int index)
{
  timer.modeChanged();
  update();
}

/*
    TimersPanel
*/

TimersPanel::TimersPanel(QWidget * parent, ModelData & model, GeneralSettings & generalSettings, Firmware * firmware,
                       QWidget * prevFocus, FilteredItemModelFactory * panelFilteredModels, CompoundItemModelFactory * panelItemModels):
  ModelPanel(parent, model, generalSettings, firmware)
{
  timersCount = firmware->getCapability(Timers);

  for (int i = 0; i < CPN_MAX_TIMERS, i < timersCount; i++) {
    timers[i] = new TimerPanel(this, model, i, generalSettings, firmware, prevFocus, panelFilteredModels, panelItemModels);
    ui->gridLayout->addWidget(timers[i], 1+i, 1);
    connect(timers[i], &TimerPanel::modified, this, &SetupPanel::modified);
    connect(timers[i], &TimerPanel::nameChanged, this, &SetupPanel::onTimerNameChanged);
    connect(this, &SetupPanel::updated, timers[i], &TimerPanel::update);
    prevFocus = timers[i]->getLastFocus();
    //  TODO more reliable method required
    QLabel *label = findChild<QLabel *>(QString("label_timer%1").arg(i + 1));
    if (label) {  //  to stop crashing if not found
      label->setProperty("index", i);
      label->setContextMenuPolicy(Qt::CustomContextMenu);
      label->setToolTip(tr("Popup menu available"));
      label->setMouseTracking(true);
      connect(label, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onTimerCustomContextMenuRequested(QPoint)));
    }
  }
}

void TimersPanel::onTimerCustomContextMenuRequested(QPoint pos)
{
  QLabel *label = (QLabel *)sender();
  selectedTimerIndex = label->property("index").toInt();
  QPoint globalPos = label->mapToGlobal(pos);

  QMenu contextMenu;
  contextMenu.addAction(CompanionIcon("copy.png"), tr("Copy"), this, SLOT(cmTimerCopy()));
  contextMenu.addAction(CompanionIcon("cut.png"), tr("Cut"), this, SLOT(cmTimerCut()));
  contextMenu.addAction(CompanionIcon("paste.png"), tr("Paste"), this, SLOT(cmTimerPaste()))->setEnabled(hasTimerClipboardData());
  contextMenu.addAction(CompanionIcon("clear.png"), tr("Clear"), this, SLOT(cmTimerClear()));
  contextMenu.addSeparator();
  contextMenu.addAction(CompanionIcon("arrow-right.png"), tr("Insert"), this, SLOT(cmTimerInsert()))->setEnabled(insertTimerAllowed());
  contextMenu.addAction(CompanionIcon("arrow-left.png"), tr("Delete"), this, SLOT(cmTimerDelete()));
  contextMenu.addAction(CompanionIcon("moveup.png"), tr("Move Up"), this, SLOT(cmTimerMoveUp()))->setEnabled(moveTimerUpAllowed());
  contextMenu.addAction(CompanionIcon("movedown.png"), tr("Move Down"), this, SLOT(cmTimerMoveDown()))->setEnabled(moveTimerDownAllowed());
  contextMenu.addSeparator();
  contextMenu.addAction(CompanionIcon("clear.png"), tr("Clear All"), this, SLOT(cmTimerClearAll()));

  contextMenu.exec(globalPos);
}

bool TimersPanel::hasTimerClipboardData(QByteArray * data) const
{
  const QClipboard * clipboard = QApplication::clipboard();
  const QMimeData * mimeData = clipboard->mimeData();
  if (mimeData->hasFormat(MIMETYPE_TIMER)) {
    if (data)
      data->append(mimeData->data(MIMETYPE_TIMER));
    return true;
  }
  return false;
}

bool TimersPanel::insertTimerAllowed() const
{
  return ((selectedTimerIndex < timersCount - 1) && (model->timers[timersCount - 1].isEmpty()));
}

bool TimersPanel::moveTimerDownAllowed() const
{
  return selectedTimerIndex < timersCount - 1;
}

bool TimersPanel::moveTimerUpAllowed() const
{
  return selectedTimerIndex > 0;
}

void TimersPanel::cmTimerClear(bool prompt)
{
  if (prompt) {
    if (QMessageBox::question(this, CPN_STR_APP_NAME, tr("Clear Timer. Are you sure?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
      return;
  }

  model->timers[selectedTimerIndex].clear();
  model->updateAllReferences(ModelData::REF_UPD_TYPE_TIMER, ModelData::REF_UPD_ACT_CLEAR, selectedTimerIndex);
  updateItemModels();
  emit modified();
}

void TimersPanel::cmTimerClearAll()
{
  if (QMessageBox::question(this, CPN_STR_APP_NAME, tr("Clear all Timers. Are you sure?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
    return;

  for (int i = 0; i < timersCount; i++) {
    model->timers[i].clear();
    model->updateAllReferences(ModelData::REF_UPD_TYPE_TIMER, ModelData::REF_UPD_ACT_CLEAR, i);
  }
  updateItemModels();
  emit modified();
}

void TimersPanel::cmTimerCopy()
{
  QByteArray data;
  data.append((char*)&model->timers[selectedTimerIndex], sizeof(TimerData));
  QMimeData *mimeData = new QMimeData;
  mimeData->setData(MIMETYPE_TIMER, data);
  QApplication::clipboard()->setMimeData(mimeData,QClipboard::Clipboard);
}

void TimersPanel::cmTimerCut()
{
  if (QMessageBox::question(this, CPN_STR_APP_NAME, tr("Cut Timer. Are you sure?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
    return;
  cmTimerCopy();
  cmTimerClear(false);
}

void TimersPanel::cmTimerDelete()
{
  if (QMessageBox::question(this, CPN_STR_APP_NAME, tr("Delete Timer. Are you sure?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
    return;

  int maxidx = timersCount - 1;
  for (int i = selectedTimerIndex; i < maxidx; i++) {
    if (!model->timers[i].isEmpty() || !model->timers[i + 1].isEmpty()) {
      memcpy(&model->timers[i], &model->timers[i+1], sizeof(TimerData));
    }
  }
  model->timers[maxidx].clear();
  model->updateAllReferences(ModelData::REF_UPD_TYPE_TIMER, ModelData::REF_UPD_ACT_SHIFT, selectedTimerIndex, 0, -1);
  updateItemModels();
  emit modified();
}

void TimersPanel::cmTimerInsert()
{
  for (int i = (timersCount - 1); i > selectedTimerIndex; i--) {
    if (!model->timers[i].isEmpty() || !model->timers[i-1].isEmpty()) {
      memcpy(&model->timers[i], &model->timers[i-1], sizeof(TimerData));
    }
  }
  model->timers[selectedTimerIndex].clear();
  model->updateAllReferences(ModelData::REF_UPD_TYPE_TIMER, ModelData::REF_UPD_ACT_SHIFT, selectedTimerIndex, 0, 1);
  updateItemModels();
  emit modified();
}

void TimersPanel::cmTimerMoveDown()
{
  swapTimerData(selectedTimerIndex, selectedTimerIndex + 1);
}

void TimersPanel::cmTimerMoveUp()
{
  swapTimerData(selectedTimerIndex, selectedTimerIndex - 1);
}

void TimersPanel::cmTimerPaste()
{
  QByteArray data;
  if (hasTimerClipboardData(&data)) {
    TimerData *td = &model->timers[selectedTimerIndex];
    memcpy(td, data.constData(), sizeof(TimerData));
    updateItemModels();
    emit modified();
  }
}

void TimersPanel::swapTimerData(int idx1, int idx2)
{
  if ((idx1 != idx2) && (!model->timers[idx1].isEmpty() || !model->timers[idx2].isEmpty())) {
    TimerData tdtmp = model->timers[idx2];
    TimerData *td1 = &model->timers[idx1];
    TimerData *td2 = &model->timers[idx2];
    memcpy(td2, td1, sizeof(TimerData));
    memcpy(td1, &tdtmp, sizeof(TimerData));
    model->updateAllReferences(ModelData::REF_UPD_TYPE_TIMER, ModelData::REF_UPD_ACT_SWAP, idx1, idx2);
    updateItemModels();
    emit modified();
  }
}

void TimersPanel::onTimerNameChanged()
{
  updateItemModels();
}


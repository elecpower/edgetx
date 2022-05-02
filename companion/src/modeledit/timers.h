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

constexpr char MIMETYPE_TIMER[] = "application/x-companion-timer";

namespace Ui {
  class Timer;
}

class AutoLineEdit;

class TimerPanel : public ModelPanel
{
    Q_OBJECT

  public:
    TimerPanel(QWidget * parent, ModelData & model, TimerData & timer, GeneralSettings & generalSettings, Firmware * firmware,
               QWidget * prevFocus, FilteredItemModelFactory * panelFilteredModels, CompoundItemModelFactory * panelItemModels);
    virtual ~TimerPanel();

    virtual void update();
    QWidget * getLastFocus();

  private slots:
    void onNameChanged();
    void onItemModelAboutToBeUpdated();
    void onItemModelUpdateComplete();
    void onCountdownBeepChanged(int index);
    void onModeChanged(int index);
    void onTimerCustomContextMenuRequested(QPoint pos);

    void cmTimerClear(bool prompt = true);
    void cmTimerClearAll();
    void cmTimerCopy();
    void cmTimerCut();
    void cmTimerDelete();
    void cmTimerInsert();
    void cmTimerPaste();
    void cmTimerMoveDown();
    void cmTimerMoveUp();
    void onTimerNameChanged();

  signals:
    void nameChanged();

  private:
    TimerData & timer;
    Ui::Timer * ui;
    void connectItemModelEvents(const FilteredItemModel * itemModel);
    int modelsUpdateCnt;

    int timersCount;
    int selectedTimerIndex;
    bool hasTimerClipboardData(QByteArray * data = nullptr) const;
    bool insertTimerAllowed() const;
    bool moveTimerDownAllowed() const;
    bool moveTimerUpAllowed() const;
    void swapTimerData(int idx1, int idx2);
};

class TimersPanel : public ModelPanel
{
    Q_OBJECT

  public:
    TimersPanel(QWidget * parent, ModelData & model, TimerData & timer, GeneralSettings & generalSettings, Firmware * firmware,
               QWidget * prevFocus, FilteredItemModelFactory * panelFilteredModels, CompoundItemModelFactory * panelItemModels);
    virtual ~TimersPanel();

    virtual void update();
    QWidget * getLastFocus();

  private slots:
    void onNameChanged();
    void onItemModelAboutToBeUpdated();
    void onItemModelUpdateComplete();
    void onCountdownBeepChanged(int index);
    void onModeChanged(int index);
    void onTimerCustomContextMenuRequested(QPoint pos);
    void cmTimerClear(bool prompt = true);
    void cmTimerClearAll();
    void cmTimerCopy();
    void cmTimerCut();
    void cmTimerDelete();
    void cmTimerInsert();
    void cmTimerPaste();
    void cmTimerMoveDown();
    void cmTimerMoveUp();
    void onTimerNameChanged();

  signals:
    void nameChanged();

  private:
    TimerPanel * timers[CPN_MAX_TIMERS];
    TimerData & timer;
    Ui::Timer * ui;
    void connectItemModelEvents(const FilteredItemModel * itemModel);
    int modelsUpdateCnt;

    int timersCount;
    int selectedTimerIndex;
    bool hasTimerClipboardData(QByteArray * data = nullptr) const;
    bool insertTimerAllowed() const;
    bool moveTimerDownAllowed() const;
    bool moveTimerUpAllowed() const;
    void swapTimerData(int idx1, int idx2);
};

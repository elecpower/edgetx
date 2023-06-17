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

#include "autocombobox.h"

AutoComboBox::AutoComboBox(QWidget * parent):
  QComboBox(parent),
  AutoWidget(),
  m_field(nullptr),
  m_rawSource(nullptr),
  m_rawSwitch(nullptr),
  m_next(0),
  m_hasModel(false)
{
  // ui signals to monitor and process
  connect(this, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &AutoComboBox::onCurrentIndexChanged);

  // param signals to monitor and process
  connect(params(), &AutoWidgetParams::offsetChanged, [=] (float val) { updateValue(); });
  connect(params(), &AutoWidgetParams::intFuncsChanged, [=] () { updateValue(); });
}

AutoComboBox::~AutoComboBox()
{
}

void AutoComboBox::clear()
{
  if (!m_hasModel) {
    setLock(true);
    QComboBox::clear();
    m_next = 0;
    setLock(false);
  }
}

void AutoComboBox::insertItems(int index, const QStringList & items)
{
  if (!m_hasModel) {
    foreach(QString item, items) {
      addItem(item);
    }
  }
}

void AutoComboBox::addItem(const QString & item)
{
  if (!m_hasModel)
    addItem(item, m_next++);
}

void AutoComboBox::addItem(const QString & item, int value)
{
  if (!m_hasModel) {
    setLock(true);
    QComboBox::addItem(item, value);
    setLock(false);
    updateValue();
  }
}

void AutoComboBox::setField(unsigned int & field, GenericPanel * panel, AutoWidgetParams * params)
{
  m_field = (int *)&field;
  m_rawSource = nullptr;
  m_rawSwitch = nullptr;
  init(panel, params);
}

void AutoComboBox::setField(int & field, GenericPanel * panel, AutoWidgetParams * params)
{
  m_field = &field;
  m_rawSource = nullptr;
  m_rawSwitch = nullptr;
  init(panel, params);
}

void AutoComboBox::setField(RawSource & field, GenericPanel * panel, AutoWidgetParams * params)
{
  m_rawSource = &field;
  m_rawSwitch = nullptr;
  m_field = nullptr;
  init(panel, params);
}

void AutoComboBox::setField(RawSwitch & field, GenericPanel * panel, AutoWidgetParams * params)
{
  m_rawSwitch = &field;
  m_rawSource = nullptr;
  m_field = nullptr;
  init(panel, params);
}

void AutoComboBox::setModel(QAbstractItemModel * model)
{
  setLock(true);

  if (model) {
    QComboBox::setModel(model);
    m_hasModel = true;
  }
  else {
    clear();
    m_hasModel = false;
  }

  setLock(false);
  updateValue();
}

void AutoComboBox::setAutoIndexes()
{
  if (!m_hasModel) {
    for (int i = 0; i < count(); ++i)
      setItemData(i, i);

    updateValue();
  }
}

void AutoComboBox::updateValue()
{
  if (!m_field && !m_rawSource && !m_rawSwitch)
    return;

  setLock(true);

  if (m_field) {
    int val = 0;

    if (params()->isBitMapped())
      val = getBitMappedValue(m_field) + params()->offset();
    else if (params()->hasIntFuncs())
      val = (params()->intReadFunc())(*m_field);
    else
      val = *m_field + params()->offset();

    setCurrentIndex(findData(val));
  }
  else if (m_rawSource)
    setCurrentIndex(findData(m_rawSource->toValue()));
  else if (m_rawSwitch)
    setCurrentIndex(findData(m_rawSwitch->toValue()));

  setLock(false);
}

void AutoComboBox::onCurrentIndexChanged(int index)
{
  if (lock() || index < 0)
    return;

  bool ok;
  int val = itemData(index).toInt(&ok);
  if (!ok)
    return;

  if (m_field) {
    if (params()->isBitMapped())
      setBitMappedValue(m_field, val - params()->offset());
    else if (params()->hasIntFuncs())
      *m_field = (params()->intWriteFunc())(val);
    else
      *m_field = val - params()->offset();
  }
  else if (m_rawSource && m_rawSource->toValue() != val)
    *m_rawSource = RawSource(val);
  else if (m_rawSwitch && m_rawSwitch->toValue() != val)
    *m_rawSwitch = RawSwitch(val);
  else
    return;

  emit currentDataChanged(val);
  dataChanged();
}

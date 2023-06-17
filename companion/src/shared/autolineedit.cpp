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

#include "autolineedit.h"

#include <QRegularExpressionValidator>

AutoLineEdit::AutoLineEdit(QWidget * parent, bool updateOnChange):
  QLineEdit(parent),
  AutoWidget(),
  m_charField(NULL),
  m_strField(nullptr)
{
  if (updateOnChange)
    connect(this, &QLineEdit::textChanged, this, &AutoLineEdit::onEdited);
  else
    connect(this, &QLineEdit::editingFinished, this, &AutoLineEdit::onEdited);
}

AutoLineEdit::~AutoLineEdit()
{
}

// TODO: remove passing len
void AutoLineEdit::setField(char * field, int len, GenericPanel * panel, AutoWidgetParams * params)
{
  QRegularExpression re("[ A-Za-z0-9_\\.\\-\\,]*");
  setValidator(new QRegularExpressionValidator(re, this));

  m_charField = field;
  initField(len, panel, params);
}

// TODO: remove passing len
void AutoLineEdit::setField(QString & field, int len, GenericPanel * panel, AutoWidgetParams * params)
{
  m_strField = &field;
  initField(len, panel, params);
}

// TODO: remove passing len
void AutoLineEdit::initField(int len, GenericPanel * panel, AutoWidgetParams * params)
{
  init(panel, params);
  if (len > 0)
    this->params()->setSize(len);
  setInputMask(this->params()->inputMask());
  paramsChanged();
}

void AutoLineEdit::paramsChanged()
{
  if (params()->size())
    setMaxLength(params()->size());
}

void AutoLineEdit::updateValue()
{
  setLock(true);

  if (m_strField)
    setText(*m_strField);
  else if (m_charField)
    setText(m_charField);

  setLock(false);
}

void AutoLineEdit::onEdited()
{
  if (lock())
    return;

  if (m_strField && *m_strField != text())
    *m_strField = text();
  else if (m_charField && strcmp(m_charField, text().toLatin1()))
    strcpy(m_charField, text().toLatin1());
  else
    return;

  emit currentDataChanged();
  dataChanged();
}
